% MATLAB script for displaying last received timestamp and device status
% Author: Karol Wojslaw (karol.wojslaw@student.manchester.ac.uk)

% Thingspeak, MQTT and system config
readChannelID = 2033438;    % Channel ID to read the data from
timezone = 'Europe/Warsaw'; % Only for display (timestamps stored as GMT)

fieldID2 = 2;   % Timestamp field
fieldID3 = 3;   % Measurement-per-packet field


% -------------------- Reading and unpacking received data -------------------- %

% Read number of measurement datapoints sent in the MQTT packet
no_datapoints = thingSpeakRead(readChannelID, Field=fieldID3, NumPoints=1)

% Read Data csv packets using thingSpeak API and convert the output into numeric arrays
tS_timestamp_field = thingSpeakRead(readChannelID, Field=fieldID2, NumPoints=1, OutputFormat='table');

% Convert frequency and timestamp field datapoints to an array of strings
timestamp_str = string(tS_timestamp_field.Time);

% Unpack and store the numeric data (for the first packet)
timestamps = unpack(timestamp_str(1), no_datapoints);


% -------------------- Data analysis & visualisation -------------------- %

last_timestamp = timestamps(no_datapoints);
last_timestamp.TimeZone = timezone;             % Set timezone in which the device operates
now = datetime("now", 'TimeZone', '+00:00');    % Get current GMT Time

p = plot(0:100, 0:100); % Create an empty plot to be used for displaying text
p.Color = '#FFFFFF';
set(gca,'XTick',[], 'YTick', []); % Turn ticks off

t = text(5, 85, "Last packet received at: ");
t.FontSize = 12;

% Convert last timestamp to string and display
last_timestamp_str = string(last_timestamp, "eeee, dd/MM/yyyy - HH:mm:ss.SSS", "en_GB");
t = text(5, 74, last_timestamp_str);
t.Color = "#b04238";
t.FontSize = 15;

t = text(5, 50, "Device status:");
t.FontSize = 12;

% Calculate time in s between now and the last timestamp
dt = -seconds(time(between(now, last_timestamp)));
% Test if the last timestamp is from a minute ago or less
if(dt < 60)
    t = text(5, 35, "Online");
    t.Color = "#a6d75b";
else
    t = text(5, 35, "Offline");
    t.Color = "#a4a2a8";
end
t.FontSize = 25;


% -------------------- Function definitions -------------------- %

% Function for unpacking a single packet (Thingspeak point) of measurements
% Param: packet frequency and timestamp string arrays
% Return: frequency and timestamp numeric arrays
function timestamp_num_arr_converted = unpack(timestamp_str, no_datapoints)
    % Unpack the data by converting csv strings into string array and then numeric array
    timestamp_str_arr = sprintf('%s,', timestamp_str);
    timestamp_num_arr = sscanf(timestamp_str_arr, '%g,', no_datapoints);

    % Convert timestamps values (modified UNIX ms) into MATLAB time format
    timestamp_num_arr = timestamp_num_arr * 100;
    timestamp_num_arr = timestamp_num_arr + 1600000000000;
    T = datetime(1970,1,1,0,0,0,0,'TimeZone','+00:00','F','uuuu-MM-dd''T''HH:mm:ss.SSS Z');
    addMS = milliseconds(timestamp_num_arr);
    timestamp_num_arr_converted = addMS + T;
end