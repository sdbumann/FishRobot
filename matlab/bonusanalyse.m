pkg load statistics

data_path  = ".//data_test1";
experiment = [0, 20, 45, 90];

set(groot,'DefaultAxesFontSize',14)
set(groot,'DefaultLineLineWidth',1.5)

% read data
data = cell(length(experiment), 4);
for phase_cnt=1:length(experiment)
    fid = 1;
    for exp_cnt=1:4
        fid = fopen(sprintf("%s//1Hz%dd%d.csv", data_path, experiment(phase_cnt), exp_cnt));
        if fid == -1
            break;
        end
        fields = textscan(fgetl(fid),"%s",'Delimiter', ',');
        lines = textscan(fid,"%f",'Delimiter', ','){:};
        data{phase_cnt, exp_cnt} = reshape(lines, 4, length(lines)/4)';
        fclose(fid);
    end
end

% calculate values
means1 = zeros(length(experiment), 5);
stds1  = zeros(length(experiment), 5);
means2 = zeros(length(experiment), 5);
stds2  = zeros(length(experiment), 5);

for phase_cnt=1:length(experiment)
    % figure(1)
    % subplot(2,2, phase_cnt)
    % plot([0, 0, 6, 6, 0], [0, 2, 2, 0, 0], 'k', "LineWidth", 2) %pool
    % title(sprintf("position for %d deg", experiment(phase_cnt)))
    % xlabel("x [m]")
    % ylabel("y [m]")
    hold on;
    for exp_cnt=1:4
        if isempty(data{phase_cnt, exp_cnt})
            break;
        end
        % extract data
        t = data{phase_cnt, exp_cnt}(:,1);
        x = data{phase_cnt, exp_cnt}(:,3);
        y = data{phase_cnt, exp_cnt}(:,4);


        % regression line
        % B = [ones(size(x)),x]\y;

        % speed analysis : downsample by DSF to ignore the oscillating
        % we drop the beginnings and the ends because they are often perturbed
        DSF = 1;
        x1 = x(50:DSF:end-50);
        y1 = y(50:DSF:end-50);
        t1 = t(50:DSF:end-50);
        v1 = sqrt(diff(x1).^2+diff(y1).^2)./diff(t1);
        means1(phase_cnt, exp_cnt) = mean(v1);
        stds1(phase_cnt, exp_cnt)  = std(v1);
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
        means2(phase_cnt, exp_cnt) = mean(v2);
        stds2(phase_cnt, exp_cnt)  = std(v2);


    end
    hold off;
    figure(1)
    hold off;
end

means1(means1==0)=NaN;
stds1(stds1==0)=NaN;
means2(means2==0)=NaN;
stds2(stds2==0)=NaN;


figure(3)
h1 = errorbar(experiment, nanmean(means1,2), sqrt(nanmean(stds1.^2,2)),'r--');
hold on
h2 = errorbar(experiment, nanmean(means2,2), sqrt(nanmean(stds2.^2,2)),'b--');
h3 = plot(experiment, means1, 'rx');
h4 = plot(experiment, means2, 'bx');
legend([h1, h2, h3(1), h4(1)], {"mean instantaneous speed", "mean average speed", "instantaneous measurements", "average measurements"})
title("mean speeds vs. phase shift")
xlabel("phase shift [deg]")
ylabel("v [m s^{-1}]")
hold off
