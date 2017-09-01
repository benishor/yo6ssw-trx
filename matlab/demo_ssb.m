function [musb,mlsb,mpbu,mpbl,mhatu,mhatl,mhatam] = demo_ssb(m,ferr) 
% Simple passband SSB model for demonstration 
% m is the baseband (modulating) signal 
% ferr is the LO tuning error at the receiver in Hz 
% musb,mlsb are the baseband USB and LSB signals 
% mpb_u, mpb_l are the passband USB, LSB signals 
% mhatam is the result of AM envelope detection on the SSB signal 
% 
% Tested with wav file male.wav (link=eng2_m, fs=8khz, 16 bits/sample) 
% from http://www.signalogic.com/index.pl?page=codec_samples 
% 
% Example run with 250 Hz LO error: 
% >> [x,fs,nbits]=wavread('male.wav'); 
% >> m = x(1:2e5); % first 25 seconds 
% >> [musb,mlsb,mpbu,mpbl,mhatu,mhatl,mhatam] = demo_ssb(m, 250); 
% >> wavplay(mhatl,fs); % recovered usb with 250 Hz error 
% >> wavplay(mhatu,fs); % recovered lsb with 250 Hz error 
% >> wavplay(mhatam,fs); % using AM envelope detection (garbled) 

% Force row-vector with even length 
m = m(:).'; 
m = m(1:2*floor(length(m)/2)); 

% Baseband transmitter model at sampling freq fsbb 
musb = myhilbert(m); 
mlsb = conj(musb); 

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
% Passband Tx at center freq fcif and sampling freq fsif 
% pick some values for baseband,IF sampling freqs and IF center freq 
fsbb=8e3; 
fsif=200e3; 

% upsample baseband signal to IF sampling freq to prep for mix 
% Note: fsif/fsbb rational 
[P,Q]=rat(fsif/fsbb); 
[m2u,bu] = resample(musb,P,Q); 
[m2l,bu] = resample(mlsb,P,Q); 

% create LO for mix (LO at 80 kHz for this example) 
fcif=80e3; 
nn = (0:length(m2u)-1); 
lo = exp(j*2*pi*fcif/fsif*nn); 

% up-mix 
mpbu = sqrt(2)*real( m2u .* lo );  % Passband USB 
mpbl = sqrt(2)*real( m2l .* lo );  % Passband LSB 

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
% Add channel impairments (use AWGN @ SNR=10dB for this example) 
snr = 10; % 10 linear ratio = 10 dB 
nscale = sqrt(var(mpbu)/10); 
noiseu = nscale*randn(size(mpbu)); 
noisel = nscale*randn(size(mpbl)); 
rxu = mpbu + noiseu; 
rxl = mpbl + noisel; 

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
% Process passband Rx at center freq fcif and sampling freq fsif 
% model Rx LO freq and phase error, ferr and phierr 
% even at freq error = 250 Hz, no problem understanding clean speech 
phierr = (0/16)*pi; 
lorx = exp(-j*(2*pi*(fcif+ferr) + phierr)/fsif*nn); 

% down-mix 
musb_iq_hi = sqrt(2)*(rxu .* lorx); 
mlsb_iq_hi = sqrt(2)*(rxl .* lorx); 

% down-sample 
musb = resample(musb_iq_hi,Q,P); 
mlsb = resample(mlsb_iq_hi,Q,P); 

% Baseband receiver model 
mhatu = real(musb); 
mhatl = real(mlsb); 

% AM envelope detection (garbled audio) 
mhatam = abs(musb); 

% << END >> 

function y = myhilbert(x) 
% Note: x is a row vector and length(x) must be even 
X=fft(x); 
W=[1 2*ones(1,length(x)/2-1) 1 zeros(1,length(x)/2-1)]; 
Y = X.*W; 
y = ifft(Y); 
return 

%[x,fs,nbits]=wavread('male.wav'); 
%m = x(1:2e5); % first 25 seconds 
% >> [musb,mlsb,mpbu,mpbl,mhatu,mhatl,mhatam] = demo_ssb(m, 250); 
% >> wavplay(mhatl,fs); % recovered usb with 250 Hz error 
% >> wavplay(mhatu,fs); % recovered lsb with 250 Hz error 
% >> wavplay(mhatam,fs); % using AM envelope detection (garbled) 
