% MATLAB script for displaying last received timestamp
% Author: Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)

% Thingspeak, MQTT and system config
no_of_packets = 1;       % Number of packets to unpack and process
readChannelID = 2033438; % Channel ID to read the data from
timezone = 'Europe/Warsaw'; % Only for display (timestamps stored as GMT)

fieldID1 = 1;   % Frequency field
fieldID2 = 2;   % Timestamp field
fieldID3 = 3;   % Measurement-per-packet field

% Processing, filtering, analysis and visualisation config 
max_f_diff = 0.01; % Set max freq diff to 10 mHz (i.e. max ROC = 0.05 Hz/s)


% -------------------- Reading and unpacking received data -------------------- %

% Read number of measurement datapoints sent in the MQTT packet
no_datapoints = thingSpeakRead(readChannelID, Field=fieldID3, NumPoints=1)

% Read Data csv packets using thingSpeak API and convert the output into numeric arrays
tS_frequency_field = thingSpeakRead(readChannelID, Field=fieldID1, NumPoints=no_of_packets, OutputFormat='table');
tS_timestamp_field = thingSpeakRead(readChannelID, Field=fieldID2, NumPoints=no_of_packets, OutputFormat='table');

% Convert frequency and timestamp field datapoints to an array of strings
frequency_str = string(tS_frequency_field.Frequency);
timestamp_str = string(tS_timestamp_field.Time);

% Unpack and store the numeric data (for the first packet)
[plot_freq, plot_time] = unpack(frequency_str(1), timestamp_str(1), no_datapoints);

% Unpack and store the numeric data (for the following n-1 packets)
for i = 2:no_of_packets
    [frequency_num_arr, timestamp_num_arr] = unpack(frequency_str(i), timestamp_str(i), no_datapoints);
    plot_time = vertcat(plot_time, timestamp_num_arr);
    plot_freq = vertcat(plot_freq, frequency_num_arr);
end


% -------------------- Initial data processing and filtering -------------------- %

% Limit the max absolute difference between two points
for i = 1:((no_of_packets*no_datapoints)-1)
    if( (abs( (plot_freq((i+1))-plot_freq(i)) )  > max_f_diff) )
        if (plot_freq((i+1)) > plot_freq(i))
            plot_freq((i+1)) = plot_freq(i) + max_f_diff;
        else
            plot_freq((i+1)) = plot_freq(i) - max_f_diff;
        end
    end
end


% -------------------- Data analysis & visualisation -------------------- %

% Calculate maximum RoCoF (df/dt)
dt = 1; % Set dt value [s]
RoCoF_max = 0; % Set initial value of RoCoF to 0 Hz/s
for i = 1:((no_of_packets*no_datapoints)-(5*dt))
    RoCoF = abs(plot_freq((i+(5*dt))) - plot_freq(i)); % df between (5*dt) points (since 5 meas/s)
    if(RoCoF > RoCoF_max) % Examine if a new max has been calculated
        RoCoF_max = RoCoF;
    end
end

p1 = plot(0:100, 0:100);
p1.Color = '#FFFFFF';
set(gca,'XTick',[], 'YTick', []);

t1 = text(5, 90, "Last measurement: " + plot_freq(no_of_packets*25) + " Hz");
t1.Color = "#1984c5";

text(5, 84, "__________________________________");

t1 = text(5, 70, "Average frequency: " + round(mean(plot_freq),3) + " Hz", Editing='on');
t2 = text(5, 60, "Max frequency: " + round(max(plot_freq),3) + " Hz");
t3 = text(5, 48, "Min frequency: " + round(min(plot_freq),3) + " Hz");
t4 = text(5, 30, "Peak difference: " + round(max(plot_freq)-min(plot_freq),3) + " Hz (Max - Min)");
t5 = text(5, 19, "Max RoCoF: " + round(RoCoF_max,2) + " Hz/s");
t6 = text(5, 10, "(df/dt, for dt =1 s)");

t1.Color = "#c23728";
t2.Color = "#c23728";
t3.Color = "#c23728";
t4.Color = "#c23728";
t5.Color = "#c23728";
t6.Color = "#a4a2a8";
t6.FontSize = 9;


% -------------------- Function definitions -------------------- %

% Function for unpacking a single packet (Thingspeak point) of measurements
% Param: packet frequency and timestamp string arrays
% Return: frequency and timestamp numeric arrays
function [frequency_num_arr,timestamp_num_arr_converted] = unpack(frequency_str, timestamp_str, no_datapoints)
    % Unpack the data by converting csv strings into string array and then numeric array
    frequency_str_arr = sprintf('%s,', frequency_str);
    timestamp_str_arr = sprintf('%s,', timestamp_str);
    frequency_num_arr = sscanf(frequency_str_arr, '%g,', no_datapoints);
    timestamp_num_arr = sscanf(timestamp_str_arr, '%g,', no_datapoints);

    % Convert timestamps values (modified UNIX ms) into MATLAB time format
    timestamp_num_arr = timestamp_num_arr * 100;
    timestamp_num_arr = timestamp_num_arr + 1600000000000;
    T = datetime(1970,1,1,0,0,0,0,'TimeZone','+00:00','F','uuuu-MM-dd''T''HH:mm:ss.SSS Z');
    addMS = milliseconds(timestamp_num_arr);
    timestamp_num_arr_converted = addMS + T;
end

% Function for conditional assignemnt (= (X>1) ? X : 1) to prevent using val < 1
% Param: value to be checked (X)
% Return: X if (X>1) or 1 otherwise
function ret = at_least_one(a)
    if (a > 1)
        ret = a;
    else
        ret = 1;
    end
end