function ref = genRef(reflist, method)

% This function takes a list of "via point" times and positions and generates a 
% trajectory (positions as a function of time, in sample periods) using either
% a step trajectory or cubic interpolation.
%
%   ref = genRef(reflist, method)
%
%   Input Arguments:
%       reflist: points on the trajectory
%       method: either 'step' or 'cubic'
%
%   Output:
%       An array ref, each element representing the reference position, in degrees,
%       spaced at time 1/f, where f is the frequency of the trajectory controller.
%       Also plots ref.
%       
%   Example usage:  ref = genRef([0, 0; 1.0, 90; 1.5, -45; 2.5, 0], 'cubic');
%   Example usage:  ref = genRef([0, 0; 1.0, 90; 1.5, -45; 2.5, 0], 'step');
%
%   The via points are 0 degrees at time 0 s; 90 degrees at time 1 s;
%   -45 degrees at 1.5 s; and 0 degrees at 2.5 s.
% 
%   Note:  the first time must be 0, and the first and last velocities should be 0.

MOTOR_SERVO_RATE = 200;     % 200 Hz motion control loop
dt = 1/MOTOR_SERVO_RATE;    % time per control cycle

[numpos,numvars] = size(reflist);

if (numpos < 2) || (numvars ~= 2)
  error('Input must be of form [t1,p1; ... tn,pn] for n >= 2.');
end
reflist(1,1) = 0;           % first time must be zero
for i=1:numpos
  if (i>2)
    if (reflist(i,1) <= reflist(i-1,1))
      error('Times must be increasing in subsequent samples.');
    end
  end
end

if strcmp(method,'cubic')  % calculate a cubic interpolation trajectory

  timelist = reflist(:,1);
  poslist = reflist(:,2);
  vellist(1) = 0; vellist(numpos) = 0;
  if numpos >= 3
    for i=2:numpos-1
      vellist(i) = (poslist(i+1)-poslist(i-1))/(timelist(i+1)-timelist(i-1));
    end
  end

  refCtr = 1;
  for i=1:numpos-1            % go through each segment of trajectory
    timestart = timelist(i); timeend = timelist(i+1); 
    deltaT = timeend - timestart;
    posstart = poslist(i); posend = poslist(i+1);
    velstart = vellist(i); velend = vellist(i+1);
    a0 = posstart;            % calculate coeffs of traj pos = a0+a1*t+a2*t^2+a3*t^3
    a1 = velstart;
    a2 = (3*posend - 3*posstart - 2*velstart*deltaT - velend*deltaT)/(deltaT^2);
    a3 = (2*posstart + (velstart+velend)*deltaT - 2*posend)/(deltaT^3);
    while (refCtr-1)*dt < timelist(i+1)
      tseg = (refCtr-1)*dt - timelist(i);
      ref(refCtr) = a0 + a1*tseg + a2*tseg^2 + a3*tseg^3;  % add an element to ref array
      refCtr = refCtr + 1;
    end
  end

else  % default is step trajectory

% convert the list of times to a list of sample numbers
  sample_list = reflist(:,1) * MOTOR_SERVO_RATE;
  angle_list = reflist(:,2);
  ref = zeros(1,max(sample_list));
  last_sample = 0;
  samp = 0;
  for i=2:numpos
    if (sample_list(i,1)  <= sample_list(i-1,1))
      error('Times must be in ascending order.');
    end
    for samp = last_sample:(sample_list(i)-1)
      ref(samp+1) = angle_list(i-1);
    end
    last_sample = sample_list(i)-1;
  end
  ref(samp+1) = angle_list(end);

end

str = sprintf('%d samples at %7.2f Hz taking %5.3f sec', ...
	      length(ref),MOTOR_SERVO_RATE,reflist(end,1));
plot(ref);
title(str);
border = 0.1*(max(ref)-min(ref));
axis([0, length(ref), min(ref)-border, max(ref)+border]);
ylabel('Motor angle (degrees)'); 
xlabel('Sample number');
set(gca,'FontSize',18);

