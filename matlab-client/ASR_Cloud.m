function varargout = ASR_Cloud(varargin)
% ASR_CLOUD MATLAB code for ASR_Cloud.fig
%      ASR_CLOUD, by itself, creates a new ASR_CLOUD or raises the existing
%      singleton*.
%
%      H = ASR_CLOUD returns the handle to a new ASR_CLOUD or the handle to
%      the existing singleton*.
%
%      ASR_CLOUD('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in ASR_CLOUD.M with the given input arguments.
%
%      ASR_CLOUD('Property','Value',...) creates a new ASR_CLOUD or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before ASR_Cloud_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to ASR_Cloud_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help ASR_Cloud

% Last Modified by GUIDE v2.5 06-Feb-2015 03:38:22

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @ASR_Cloud_OpeningFcn, ...
                   'gui_OutputFcn',  @ASR_Cloud_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before ASR_Cloud is made visible.
function ASR_Cloud_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to ASR_Cloud (see VARARGIN)

% Choose default command line output for ASR_Cloud
handles.output = hObject;

handles.N = 0;
handles.fs = 16000;
handles.A = [];
handles.Y = [];
handles.DATALEN = 1024;
PORT = 4950;
SERVERIP = '128.199.123.155';
handles.t = tcpip(SERVERIP, PORT, 'NetworkRole', 'client', 'OutputBufferSize', 1024);

set(handles.startbutton, 'Enable', 'off');
% Update handles structure
guidata(hObject, handles);

% UIWAIT makes ASR_Cloud wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = ASR_Cloud_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in startbutton.
function startbutton_Callback(hObject, eventdata, handles)
% hObject    handle to startbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

try
    fopen(handles.t);
catch err
    set(handles.statustext, 'String', 'Error open connection! The server may not run!');
    return;
end

fwrite(handles.t, typecast(int32(handles.N), 'uint8'));

player = audioplayer(handles.A, handles.fs);

set(handles.statustext, 'String', 'Recognizing...');
play(player);
ii = 1;
while(ii <= handles.N)
   if (handles.N - ii + 1 < handles.DATALEN)
       slen = handles.N - ii + 1;
   else
       slen = handles.DATALEN;
   end
   fwrite(handles.t, handles.Y(ii : ii+slen-1));
   ii = ii + slen;
   nbytes = 0;
   while nbytes == 0
       nbytes = handles.t.BytesAvailable;
   end
   hyp = '>>> ';
   while nbytes
       [temp, n] = fread(handles.t, nbytes, 'uint8');
       hyp = [hyp temp'];
       nbytes = nbytes - n;
   end
   set(handles.result, 'String', hyp);
   drawnow
end

fclose(handles.t);
set(handles.statustext, 'String', 'Finish! You can load a new file now!');



function filenametext_Callback(hObject, eventdata, handles)
% hObject    handle to filenametext (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of filenametext as text
%        str2double(get(hObject,'String')) returns contents of filenametext as a double


% --- Executes during object creation, after setting all properties.
function filenametext_CreateFcn(hObject, eventdata, handles)
% hObject    handle to filenametext (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in loadbutton.
function loadbutton_Callback(hObject, eventdata, handles)
% hObject    handle to loadbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
filename = get(handles.filenametext, 'String');
if(isempty(filename))
   set(handles.statustext, 'String', 'Error! Empty file name!');
else if exist(filename, 'file')
    [handles.A, handles.fs] = wavread(filename);
    handles.Y = typecast(int16(handles.A * 2^15), 'uint8');
    handles.N = size(handles.Y, 1);
    set(handles.startbutton, 'Enable', 'on');
    set(handles.statustext, 'String', 'Load file successfully!');
    else
        set(handles.statustext, 'String', 'File does not exist!');
    end
end
guidata(hObject, handles);
