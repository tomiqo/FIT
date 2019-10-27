% 1. [0.5 bodu] Nacitanie osobneho signalu zo suboru xzigot00.wav. Vypis
% vzorkovacej frekvencie signálu, dlzky vo vzorkoch a v sekundach a pocet
% reprezentovanych binarnych symbolov (1 symbol odpoveda 16 vzorkom).
[Y,Fs] = audioread('xzigot00.wav');
fprintf('Vzorkovacia frekvencia: %f [Hz].\n', Fs);
Y = Y';
length_s = length(Y);
fprintf('Dlzka signalu vo vzorkach: %f.\n', length_s);
fprintf('Dlzka signalu v sekundach: %f [s].\n', length_s / Fs);
fprintf('Pocet binarnych symbolov: %f.\n', length_s / 16);

% 2. [1 bod] Dekodovanie do binarnych symbolov. a porovnanie so suborom
% xzigot00.txt 
figure('Name','Signal so vzorkami','NumberTitle','off');
line = 8;
cnt = 1;
tmp = zeros(1,(length_s / 16));
decoded = NaN(1,length_s);
while line < length_s
    if Y(line) > 0
        %vystup je 1
        tmp(cnt) = 1; 
    elseif Y(line) < 0
        %vystup je 0
        tmp(cnt) = 0;
    end
    decoded(line) = tmp(cnt);
    cnt = cnt + 1;
    line = line + 16;
end
t=1:length_s;
t=t/Fs;
yaxis=zeros(1,length_s);
hold on;
plot(t(1:320),Y(1:320));
plot(t(1:320),decoded(1:320),'o');
plot(t(1:320),yaxis(1:320));
grid;
title('Excercise 2');
xlabel('t');
ylabel('s[n],symbols');
hold off;
ff = fopen('xzigot00.txt','r');
s = fread(ff, [1 inf],'int8', 1);
i = 1;
while i < 2001
    s(i)=s(i)-48;
    i=i+1;
end
if s == tmp
    fprintf('Dekodovany kod sa zhoduje s kodom v subore.\n');
else
    fprintf('Dekodovany kod sa nezhoduje s kodom v subore.\n');
end



%3. [0.5 bodu] Je dany filter, vykreslit jeho obrazok s nulovymi bodmi a
%polmy prenosovej funkcie tohto filtru a uviest ci je stabilny
figure('Name','Obrazok filtra','NumberTitle','off');
B = [0.0192 -0.0185 -0.0185 0.0192];
A = [1.0000 -2.8870 2.7997 -0.9113];
zplane (B,A);
title('Excercise 3');
p = roots(A); 
if (isempty(p) | abs(p) < 1) 
  fprintf('Filter je stabilny.\n');
else
  fprintf('Filter je nestabilny.\n');
end

%4. [0.5 bodu] Modul kmitoctovej char, +uviest typ filtru a kde lezi mezna
%frekvencia v Hz
figure('Name','Modul kmitoctovej char','NumberTitle','off');
H = abs(freqz(B,A,256)); f=(0:255) / 256 * Fs / 2; 
[minHValue,minHX] = min(H(1:25));
hold on;
plot (f,H);
plot (f(minHX),minHValue,'o');
hold off;
grid; xlabel('f'); ylabel('|H(f)|');
title('Excercise 4');
xlabel('f [Hz]'); ylabel ('PSD [dB]');

%5. [1 bod] Filtrovanie signalu Y a nasledne vykreslenie
figure('Name','Filtrovany signal Y','NumberTitle','off');
signalf = filter(B,A,Y);
hold on;
plot(t(1:320),Y(1:320));
plot(t(1:320),signalf(1:320));
hold off;
xlabel('t');
ylabel('s[n],ss[n]');
title('Excercise 5');

%6. [1 bod]
figure('Name','Signaly aj s posunutym','NumberTitle','off');
signalfs = circshift (signalf,-17);
line = 8;
cnt = 1;
tmp2 = zeros(1,(length_s / 16));
decoded2 = NaN(1,length_s);
zerocnt = 0;
while line < length_s
    if signalfs(line) > 0
        %vystup je 1
        tmp2(cnt) = 1; 
    elseif signalfs(line) < 0
        %vystup je 0
        tmp2(cnt) = 0;
    end
    decoded2(line) = tmp2(cnt);
    if tmp2(cnt) ~= tmp (cnt)
        zerocnt = zerocnt +1;
    end
    cnt = cnt + 1;
    line = line + 16;
end
hold on;
plot(t(1:320),Y(1:320));
plot(t(1:320),decoded2(1:320),'o');
plot(t(1:320),yaxis(1:320));
plot(t(1:320),signalf(1:320));
plot(t(1:320),signalfs(1:320));
grid;
title('Excercise 6');
xlabel('t');
ylabel('s[n],ss[n],ss_{shifte}[n],symbols');
hold off;

%7. [1 bod]
compared =(zerocnt / (length_s / 16) * 100);
fprintf('Pocet chybnych symbolov je: %d.\n',zerocnt);
fprintf('Posunuty signal ma percentualnu chybovost: %f%%.\n',compared);

%8. [1 bod] Vypocitanie spektier z oboch signalov pomocou disktretnej
%Fourierovej transformacie
figure('Name','Modul spektier v zavislosti na frekvencii','NumberTitle','off');
fft_signal = abs(fft(Y));
fft_signalf = abs(fft(signalf));
f = (0:length_s/2-1)/length_s * Fs;
hold on;
plot (f,fft_signal(1:length_s/2)); 
plot (f,fft_signalf(1:length_s/2)); 
hold off;
xlabel('f [Hz]'); ylabel ('PSD [dB]'); grid; 
title('Excercise 8');

%9. [1 bod]  Odhad funkcie hustoty rozdelenia pravdepodobnosti
figure('Name','Odhad funkcie hustoty rozdelenia pravdepodobnosti','NumberTitle','off');
gmin = min(min(Y)); 
gmax = max(max(Y));
kolik = 200;
g = linspace(gmin,gmax,kolik);
deltag = g(2) - g(1);
pg = hist(Y,g) / length_s / deltag;
plot (g,pg);
title('Excercise 9');
dens = sum(pg*deltag);
if round(dens) == 1
    fprintf('Jedna sa o spravne odhadhnutu funkciu hustoty rozdelenia pravdepodobnosti.\n');
else
    fprintf('Funkcia hustoty rozdelenia pravdepodobnosti nie je spravna\n');
end
 
%10. [0.5 bodu] Spocitanie korelacnych koeficientov 
figure('Name','Koleracne koeficienty R[k]','NumberTitle','off');
k = -50:50;
Rv = xcorr(Y,'biased');
Rv = Rv(k+length_s);
plot(k,Rv)
xlabel('k');
grid;
title('Excercise 10');

%11. [0.5 bodu] Hodnoty koeficientov R[0], R[1], a R[16]
fprintf('Koeficient R[0] ma hodnotu %f.\n', Rv(51));
fprintf('Koeficient R[1] ma hodnotu %f.\n', Rv(52));
fprintf('Koeficient R[16] ma hodnotu %f.\n', Rv(67));

%12. [2 body] Casovy odhad medzi vzorkami n a n+1
figure('Name','Casovy odhad medzi vzorkami n a n+1','NumberTitle','off');
L = 50;
h = zeros(L,L);
x = linspace(min(Y), max(Y), 50);
[~, ind1] = min(abs(repmat(Y(:)', L, 1) - repmat(x(:), 1, length_s)));
ind2 = ind1(1+1:length_s);
for ii=1:length_s-1,
  d1 = ind1(ii); d2 = ind2(ii); 
  h(d1,d2) = h(d1,d2) + 1; 
end
surf = (x(2) - x(1))^2;
p = h / length_s / surf;  
imagesc(x, x, p);
axis xy; colorbar;
xlabel('x2'); ylabel ('x1');
title('Excercise 12');

%13. [1 bod] Overenie spravnosti funkcie
check = sum(sum(p))*surf;
if round(check) == 1
    fprintf('Jedna sa o spravnu zdruzenu funkciu hustoty rozdelenia pravdepodobnosti.\n');
else
    fprintf('Jedna sa o NEspravnu!! zdruzenu funkciu hustoty rozdelenia pravdepodobnosti.\n');
end
fprintf('Hodnota integralu je: %f.\n',check);

%14. [1 bod] Vypocitanie z odhadnutej rovnice koeficient R[1]
x = x(:); X1 = repmat(x,1,L);
x = x'; X2 = repmat(x,L,1); 
r = sum(sum (X1 .* X2 .* p)) * surf;
fprintf('Koeficient R[1] ma hodnotu %f.\n', r);
