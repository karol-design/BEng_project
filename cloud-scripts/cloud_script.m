% MATLAB script for visualizing data from a single HertzNet device

% Number of packets to display on a single graph
no_of_packets = 80;

% Channel ID to read data from:
readChannelID = 2033438;

% Field ID to read data from:
fieldID1 = 1;
fieldID2 = 2;
fieldID3 = 3;

% Read number of measurement datapoints sent in the MQTT packet
no_datapoints = thingSpeakRead(readChannelID, Field=fieldID3, NumPoints=1)

%% Read Data csv packets using thingSpeak API and convert the output into numeric arrays %%
tS_frequency_field = thingSpeakRead(readChannelID, Field=fieldID1, NumPoints=no_of_packets, OutputFormat='table');
tS_timestamp_field = thingSpeakRead(readChannelID, Field=fieldID2, NumPoints=no_of_packets, OutputFormat='table');

frequency_str = string(tS_frequency_field.Frequency);
timestamp_str = string(tS_timestamp_field.Time);

frequency_str_arr = sprintf('%s,', frequency_str(1));
timestamp_str_arr = sprintf('%s,', timestamp_str(1));

frequency_num_arr = sscanf(frequency_str_arr, '%g,', no_datapoints);
timestamp_num_arr = sscanf(timestamp_str_arr, '%g,', no_datapoints);

timestamp_num_arr = timestamp_num_arr * 100;
timestamp_num_arr = timestamp_num_arr + 1600000000000;

% Convert UNIX ms timestamps from MQTT packet to MATLAB time format
T = datetime(1970,1,1,0,0,0,0,'TimeZone','UTC','F','uuuu-MM-dd''T''HH:mm:ss.SSS Z');
addMS = milliseconds(timestamp_num_arr);
timestamp_num_arr_converted = addMS + T;
 
plot_time = timestamp_num_arr_converted;
plot_freq = frequency_num_arr;

for i = 2:no_of_packets
    frequency_str_arr = sprintf('%s,', frequency_str(i));
    timestamp_str_arr = sprintf('%s,', timestamp_str(i));

    frequency_num_arr = sscanf(frequency_str_arr, '%g,', no_datapoints);
    timestamp_num_arr = sscanf(timestamp_str_arr, '%g,', no_datapoints);

    timestamp_num_arr = timestamp_num_arr * 100;
    timestamp_num_arr = timestamp_num_arr + 1600000000000;

    % Convert UNIX ms timestamps from MQTT packet to MATLAB time format
    T = datetime(1970,1,1,0,0,0,0,'TimeZone','UTC','F','uuuu-MM-dd''T''HH:mm:ss.SSS Z');
    addMS = milliseconds(timestamp_num_arr);
    timestamp_num_arr_converted = addMS + T;
    
    plot_time = vertcat(plot_time, timestamp_num_arr_converted);
    plot_freq = vertcat(plot_freq, frequency_num_arr);
end

for i = 1:((no_of_packets*no_datapoints)-1)
    i_p = i+1;
    if( (abs( (plot_freq(i_p)-plot_freq(i)) )  > 0.01) )
        if (plot_freq(i_p) > plot_freq(i))
            plot_freq(i_p) = plot_freq(i) + 0.01;
        end
        if (plot_freq(i_p) < plot_freq(i))
            plot_freq(i_p) = plot_freq(i) - 0.01;
        end
    end
end

%k = no_of_packets / 100;
%plot_freq_moving_avg = movmean(plot_freq, k);
freq_mean = mean(plot_freq);
N = no_of_packets*no_datapoints;
plot_freq_mean = zeros(N,1) + freq_mean;

    
%% Visualize Data %%
freq_max = max(plot_freq) + 0.03;
freq_min = min(plot_freq) - 0.03;
p1 = plot(plot_time, plot_freq);
p1.Color = '#D95319';
xlabel('Time');
ylabel('Frequency [Hz]');

hold on;

p2 = plot(plot_time, plot_freq_mean, ':');
p2.Color = '#FF0000';
ylim([freq_min freq_max]);
