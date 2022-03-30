clc;
close all;
clear;

data_table=readtable('\data_test1\0_5Hz1.csv');
data=table2array(data_table);
x=data(:,3);
y=data(:,4);
plot(x,y);
axis([0 6 0 2]);