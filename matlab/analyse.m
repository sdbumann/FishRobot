% read file
fid = fopen('data_test1//0_5Hz2.csv');
fields = textscan(fgetl(fid),"%s",'Delimiter', ',');
lines = textscan(fid,"%f",'Delimiter', ','){:};
data = reshape(lines, 4, length(lines)/4)';
fclose(fid);

t = data(:,1);
x = data(:,3);
y = data(:,4);

% linear regression
B = [ones(size(x)),x]\y;

% speed analysis
v = sqrt(diff(x).^2+diff(y).^2)./diff(t);
disp(mean(v))
disp(std(v))

% plot position [6x2 m]
figure(1);
title("position")
plot([0, 0, 6, 6, 0], [0, 2, 2, 0, 0], 'k', "LineWidth", 2) %pool
hold on;
plot(x, y, "LineWidth", 2);
plot([0:.1:6], B(1)+B(2)*[0:.1:6], 'r--')
axis("equal")
xlabel("x [m]")
ylabel("y [m]")
hold off;

% plot speed
figure(2)
title("speed")
plot(t(2:end), v, "LineWidth", 2)
hold on
plot(t(2:end), mean(v)*ones(size(v)), 'r')
plot(t(2:end), (mean(v)+std(v))*ones(size(v)), 'r--')
plot(t(2:end), (mean(v)-std(v))*ones(size(v)), 'r--')
xlabel("t [s]")
ylabel("v [m s^{-1}]")
hold off
