% Compute the FFT of a signal and FIR filter the signal in both MATLAB and on the PIC32 
% and compare the results
% open the serial port
port ='/dev/tty.usbserial-00001014A';  % modify for your own port

if ~isempty(instrfind)  % closes the port if it was open
  fclose(instrfind);
  delete(instrfind);
end
fprintf('Opening serial port %s\n',port);
ser = serial(port, 'BaudRate', 230400, 'FlowControl','hardware');
fopen(ser);

% generate the input signal
xp(1:50) = 200; 
xp(51:100)= 0;
x = [xp xp xp xp xp xp xp xp xp xp 200*ones(1,24)];

% now, create the FIR filter
Wn = 0.02; % let's see if we can just get the lowest frequency sinusoid
            
ord = 47; % ord+1 must be a multiple of 4
fir_coeff = fir1(ord,Wn);

N = length(x);   
Y = fft(x);    % computer MATLAB's fft

xfil = filter(fir_coeff,1,x); % filter the signal
Yfil = fft(xfil);             % fft the filtered signal

% generate data for FFT plots for the original signal
mag = 2*abs(Y(1:N/2+1))/N; 
mag(1) = mag(1)/2; 
mag(N/2+1) = mag(N/2+1)/2;

% generate data for FFT plots for the filtered signal
magfil = 2*abs(Yfil(1:N/2+1))/N; 
magfil(1) = magfil(1)/2; 
magfil(N/2+1) = magfil(N/2+1)/2;

freqs = linspace(0,1,N/2+1); 

% send the original signal to the pic32
fprintf(ser,'%d\n',N); % send the length
for i=1:N 
  fprintf(ser,'%f\n',x(i)); % send each sample in the signal
end

% send the fir filter coefficients
fprintf(ser,'%d\n',length(fir_coeff));
for i=1:length(fir_coeff)
  fprintf(ser,'%f\n',fir_coeff(i));
end

% now we can read in the values sent from the PIC.
elapsedns = fscanf(ser,'%d');
disp(['The first 1024-sample FFT took ',num2str(elapsedns/1000.0),' microseconds.']);
Npic = fscanf(ser,'%d');
data = zeros(Npic,4); % the columns in data are
                      % original signal, fir filtered, orig fft, fir fft
for i=1:Npic
  data(i,:) = fscanf(ser,'%f %f %f %f');
end

xpic = data(:,1);           % original signal from the pic
xfirpic = data(:,2);        % fir filtered signal from pic
Xfftpic = data(1:N/2+1,3);  % fft signal from the pic
Xfftfir = data(1:N/2+1,4);  % fft of filtered signal from the pic

                            % used to plot the fft pic signals
Xfftpic = 2*abs(Xfftpic);
Xfftpic(1) = Xfftpic(1)/2;
Xfftpic(N/2+1) = Xfftpic(N/2+1)/2;

Xfftfir = 2*abs(Xfftfir);
Xfftfir(1) = Xfftfir(1)/2;
Xfftfir(N/2+1) = Xfftfir(N/2+1)/2;

% now we are ready to plot
subplot(3,1,1);
hold on;
title('Plot of the original signal and the FIR filtered signal')
xlabel('Sample number')
ylabel('Amplitude')
plot(x,'Marker','o');
plot(xfil,'Color','red','LineWidth',2);
plot(xfirpic,'o','Color','black');
hold off;
legend('Original Signal','MATLAB FIR', 'PIC FIR')
axis([-10,1050,-10,210])
set(gca,'FontSize',18);

subplot(3,1,2);
hold on;
title('FFTs of the original signal')
ylabel('Magnitude')
xlabel('Normalized frequency (fraction of Nyquist Frequency)')
stem(freqs,mag)
stem(freqs,Xfftpic,'Color','black')
legend('MATLAB FFT', 'PIC FFT')
hold off;
set(gca,'FontSize',18);

subplot(3,1,3);
hold on;
title('FFTs of the filtered signal')
ylabel('Magnitude')
xlabel('Normalized frequency (fraction of Nyquist Frequency)')
stem(freqs,magfil)
stem(freqs,Xfftfir,'Color','black')
legend('MATLAB FFT', 'PIC FFT')
hold off;
set(gca,'FontSize',18);

fclose(ser);
