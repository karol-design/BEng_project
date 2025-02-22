% MATLAB script for reading, unpacking and displaying data from a single HertzNet device
% Author: Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)

% Thingspeak, MQTT and system config
no_of_packets = 4320;       % Number of packets to unpack and process
readChannelID = 2033438;    % Channel ID to read the data from
timezone = 'Europe/Warsaw'; % Only for display (timestamps stored as GMT)

fieldID1 = 1;   % Frequency field
fieldID2 = 2;   % Timestamp field
fieldID3 = 3;   % Measurement-per-packet field

% Processing, filtering, analysis and visualisation config 
max_f_diff = 0.01; % Set max freq diff to 10 mHz (i.e. max ROC = 50 mHz/s)


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


% -------------------- Data analysis -------------------- %
step = at_least_one(uint64(1))
data = [plot_time(1:step:end), plot_freq(1:step:end)]


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