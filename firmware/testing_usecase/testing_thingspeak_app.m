% Template MATLAB code for reading data from a private channel, analyzing
% the data and storing the analyzed data in another channel.

% Prior to running this MATLAB code template, assign the channel ID to read
% data from to the 'readChannelID' variable. Since this is a private
% channel, also assign the read API Key to the 'readAPIKey' variable. You
% can find the read API Key in the right side pane of this page.

% To store the analyzed data, you will need to write it to a channel other
% than the one you are reading data from. Assign this channel ID to the
% 'writeChannelID' variable. Also assign the write API Key to the
% 'writeAPIKey' variable below. You can find the write API Key in the right
% side pane of this page.

%  modified by Julien JACQUES - Fablab de Lannion
%
%  author: jjacques@legtux.org
%  date  : 19-oct-2016
%  desc  : get 'deep' last values from 'readChannelID'
%          compute data and get a score between [0,2](integer)
%          write the score in 'writeChannelID'
%  notes : 

% TODO - Replace the [] with channel ID to read data from:
readChannelID = [];
% TODO - Enter the Read API Key between the '' below:
readAPIKey = '';

% TODO - Replace the [] with channel ID to write data to:
writeChannelID = [];
% TODO - Enter the Write API Key between the '' below:
writeAPIKeyAvg = '';
readAPIKeyAvg  = '';

deep=10;

%% Read Data %%
%analyzedData=-1;
data = thingSpeakRead(readChannelID, 'ReadKey', readAPIKey, 'NumPoints',deep);
analyzedData=thingSpeakRead(writeChannelID, 'ReadKey', readAPIKeyAvg, 'NumPoints',1);
disp(data);
lastdata=data(deep);
disp(lastdata);
disp(analyzedData);

%% Analyze Data %%
% Add code in this section to analyze data and store the result in the
% analyzedData variable.
%disp(data);
meanData = mean(data);
disp(meanData);

%if(meanData<45) analyzedData=0;
%elseif(meanData>48) analyzedData=2;
%else analyzedData=1;
%end
%& (meanData>lastdata-10)
%| (meanData<lastdata+10) 

if( ( lastdata+meanData<90 ) & (analyzedData>0) ) analyzedData=analyzedData-1;
elseif ( ( lastdata+meanData>90  ) & (analyzedData>=0) & (analyzedData<2) ) analyzedData=analyzedData+1;
elseif ( analyzedData<0 & lastdata<40) analyzedData=0;
elseif ( analyzedData<0 & lastdata<60) analyzedData=1;
elseif ( analyzedData<0 & lastdata>=60) analyzedData=2;
elseif ( analyzedData>2 ) analyzedData=2;
else
    %no color transition
end

disp(analyzedData);

%% Write Data %%
thingSpeakWrite(writeChannelID, analyzedData, 'WriteKey', writeAPIKeyAvg);