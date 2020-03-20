function pidtest(Kp, Ki, Kd)

INERTIA = 0.5;        % The plant is a link attached to a revolute joint
MASS = 1;             % hanging in GRAVITY, and the output is the angle of the joint.
CMDIST = 0.1;         % The link has INERTIA about the joint, MASS center at CMDIST 
DAMPING = 0.1;        % from the joint, and there is frictional DAMPING.
GRAVITY = 9.81;
DT = 0.001;           % timestep of control law
NUMSAMPS = 1001;      % number of control law iterations
UMAX = 20;            % maximum joint torque by the motor

eprev = 0;
eint = 0;
r = 1;                % reference is constant at one radian
vel = 0;              % velocity of the joint is initially zero
s(1) = 0.0; t(1) = 0; % initial joint angle and time
for i=1:NUMSAMPS
  e = r - s(i);         
  edot = e - eprev;
  eint = eint + e;
  u(i) = Kp*e + Ki*eint + Kd*edot;
  if (u(i) > UMAX)
    u(i) = UMAX;
  elseif (u(i) < -UMAX)
    u(i) = -UMAX;
  end
  eprev = e;
  t(i+1) = t(i) + DT;

  % acceleration due to control torque and dynamics
  acc = (u(i) - MASS*GRAVITY*CMDIST*sin(s(i)) - DAMPING*vel)/INERTIA;

  % a simple numerical integration scheme
  s(i+1) = s(i) + vel*DT + 0.5*acc*DT*DT;
  vel = vel + acc*DT;
end

plot(t(1:NUMSAMPS),r-s(1:NUMSAMPS),'Color','black');
hold on;
plot(t(1:NUMSAMPS),u/20,'--','Color','black');
set(gca,'FontSize',18);
legend({'e(t)','u(t)/20'},'FontSize',18);
plot([t(1),t(length(t)-1)],[0,0],':','Color','black');
axis([0 1 -1.1 1.1])
title(['Kp: ',num2str(Kp),'  Ki: ',num2str(Ki),'  Kd: ',num2str(Kd)]);
hold off

