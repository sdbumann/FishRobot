pkg load statistics
pkg load signal

clc
close all

data_path  = ".//data_test1";
experiment = {"0_5", "0_75", "1", "1_25", "1_5"};

set(groot,'DefaultAxesFontSize',14)
set(groot,'DefaultLineLineWidth',1.5)

% read data
data = cell(length(experiment), 5);
for freq_cnt=1:length(experiment)
    fid = 1;
    for exp_cnt=1:5
        fid = fopen(sprintf("%s//%sHz%d.csv", data_path, experiment{freq_cnt},exp_cnt));
        if fid == -1
            break;
        end
        fields = textscan(fgetl(fid),"%s",'Delimiter', ',');
        lines = textscan(fid,"%f",'Delimiter', ','){:};
        data{freq_cnt, exp_cnt} = reshape(lines, 4, length(lines)/4)';
        fclose(fid);
    end
end

% calculate values
means1 = zeros(length(experiment), 5);
stds1  = zeros(length(experiment), 5);
means2 = zeros(length(experiment), 5);
stds2  = zeros(length(experiment), 5);

for freq_cnt=1:length(experiment)
    % figure(1)
    % subplot(3,2, freq_cnt)
    % plot([0, 0, 6, 6, 0], [0, 2, 2, 0, 0], 'k', "LineWidth", 2) %pool
    % title(sprintf("position for %s Hz", strrep(experiment{freq_cnt}, "_", ".")))
    % xlabel("x [m]")
    % ylabel("y [m]")
    % hold on
    % figure(3)
    % subplot(3,2, freq_cnt)
    % plot([0, 0, 6, 6, 0], [0, 2, 2, 0, 0], 'k', "LineWidth", 2) %pool
    % title(sprintf("position for %s Hz", strrep(experiment{freq_cnt}, "_", ".")))
    % xlabel("x [m]")
    % ylabel("y [m]")
    hold on;
    for exp_cnt=1:5
        if isempty(data{freq_cnt, exp_cnt})
            break;
        end
        % extract data
        t = data{freq_cnt, exp_cnt}(:,1);
        x = data{freq_cnt, exp_cnt}(:,3);
        y = data{freq_cnt, exp_cnt}(:,4);


        % regression line
        % B = [ones(size(x)),x]\y;

        % speed analysis : downsample by DSF to ignore the oscillating
        % we drop the beginnings and the ends because they are often perturbed
        DSF = 1;
        x1 = x(50:DSF:end-50);
        y1 = y(50:DSF:end-50);
        t1 = t(50:DSF:end-50);
        v1 = sqrt(diff(x1).^2+diff(y1).^2)./diff(t1);
        means1(freq_cnt, exp_cnt) = mean(v1);
        stds1(freq_cnt, exp_cnt)  = std(v1);
        % figure(1)
        % plot(x1, y1, "LineWidth", 2);
        % % plot([0:.1:6], B(1)+B(2)*[0:.1:6], 'r--')
        % axis("equal")
        %
        % figure(2)
        % subplot(3,2, freq_cnt)
        % plot(t1(2:end), v1, "LineWidth", 2)
        % title(sprintf("speed at %s Hz", strrep(experiment{freq_cnt}, "_", ".")))
        % xlabel("t [s]")
        % ylabel("v [m s^{-1}]")
        % hold on;

        % smoothed
        g = ones(30,1)/30;
        x2 = filter(g,1, x);
        y2 = filter(g,1, y);
        x2 = x2(50:end-50);
        y2 = y2(50:end-50);
        t2 = t(50:end-50);

        v2 = sqrt(diff(x2).^2+diff(y2).^2)./diff(t2);
        means2(freq_cnt, exp_cnt) = mean(v2);
        stds2(freq_cnt, exp_cnt)  = std(v2);


        % figure(3)
        % plot(x2, y2, "LineWidth", 2);
        % % plot([0:.1:6], B(1)+B(2)*[0:.1:6], 'r--')
        % axis("equal")
        %
        % figure(4)
        % subplot(3,2, freq_cnt)
        % plot(t2(2:end), v2, "LineWidth", 2)
        % title(sprintf("speed at %s Hz", strrep(experiment{freq_cnt}, "_", ".")))
        % xlabel("t [s]")
        % ylabel("v [m s^{-1}]")
        % hold on;

    end
    hold off;
    figure(1)
    hold off;
    figure(2)
    hold off;
    figure(3)
    hold off;
end

means1(means1==0)=NaN;
stds1(stds1==0)=NaN;
means2(means2==0)=NaN;
stds2(stds2==0)=NaN;

figure(5)
f = .25+[1:5]/4;
h1 = errorbar(f, nanmean(means1,2), sqrt(nanmean(stds1.^2,2)),'r--');
hold on
h2 = errorbar(f, nanmean(means2,2), sqrt(nanmean(stds2.^2,2)),'b--');
h3 = plot(f, means1, 'rx');
h4 = plot(f, means2, 'bx');
hold off
legend([h1, h2, h3(1), h4(1)], {"mean instantaneous speed", "mean average speed", "instantaneous measurements", "average measurements"})
title("mean speeds vs. frequency")
xlabel("f [Hz]")
ylabel("v [m s^{-1}]")



m=(reshape(means2, 1,25)(1:end-3))';
f=(reshape([0.5, 0.75, 1, 1.25, 1.5]'.*ones(5,5), 1, 25)(1:end-3))';
B = ([ones(size(f)),f])\m;

figure(5)
hold on
fs=[.5:.5:1.5];
plot(fs, B(1)+B(2)*fs)
