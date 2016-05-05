% Demo to find certain shapes in an image based on their shape.
clc;    % Clear the command window.
close all;  % Close all figures (except those of imtool.)
imtool close all;  % Close all imtool figures.
clear;  % Erase all existing variables.
workspace;  % Make sure the workspace panel is showing.
fontSize = 20;

% Read in image into an array.
rgbImage = imread('day1.jpg'); 
[rows, columns, numberOfColorBands] = size(rgbImage); 
% Display it.
subplot(2, 2, 1);
imshow(rgbImage, []);
title('Input Image', 'FontSize', fontSize);
% Enlarge figure to full screen.
set(gcf, 'units','normalized','outerposition',[0 0 1 1]);
% Give a name to the title bar.
set(gcf,'name','Shape Recognition Demo','numbertitle','off') 

% If it's monochrome (indexed), convert it to color. 
if numberOfColorBands > 1
	grayImage = rgbImage(:,:,2);
else
	% It's already a gray scale image.
	grayImage = rgbImage;
end

% Make a triangle on it.
triangleXCoordinates = [360 420 480];
triangleYCoordinates = [350 252 350];
traiangleBinaryImage = poly2mask(triangleXCoordinates, triangleYCoordinates, rows, columns);
% Burn it into the gray scale image.
%grayImage(traiangleBinaryImage) = 255;

% Display it.
subplot(2, 2, 2);
imshow(grayImage, []);
title('Grayscale Image', 'FontSize', fontSize);

% Binarize the image.
binaryImage = grayImage > 120;
% Display it.
subplot(2, 2, 3);
imshow(binaryImage, []);
title('Initial (Noisy) Binary Image', 'FontSize', fontSize);

% Remove small objects.
binaryImage = bwareaopen(binaryImage, 300);
% Display it.
subplot(2, 2, 4);
imshow(binaryImage, []);
title('Cleaned Binary Image', 'FontSize', fontSize);

[labeledImage numberOfObjects] = bwlabel(binaryImage);
blobMeasurements = regionprops(labeledImage,...
	'Perimeter', 'Area', 'FilledArea', 'Solidity', 'Centroid'); 

% Get the outermost boundaries of the objects, just for fun.
filledImage = imfill(binaryImage, 'holes');
boundaries = bwboundaries(filledImage);

% Collect some of the measurements into individual arrays.
perimeters = [blobMeasurements.Perimeter];
areas = [blobMeasurements.Area];
filledAreas = [blobMeasurements.FilledArea];
solidities = [blobMeasurements.Solidity];
% Calculate circularities:
circularities = perimeters .^2 ./ (4 * pi * filledAreas);
% Print to command window.
fprintf('#, Perimeter,        Area, Filled Area, Solidity, Circularity\n');
for blobNumber = 1 : numberOfObjects
	fprintf('%d, %9.3f, %11.3f, %11.3f, %8.3f, %11.3f\n', ...
		blobNumber, perimeters(blobNumber), areas(blobNumber), ...
		filledAreas(blobNumber), solidities(blobNumber), circularities(blobNumber));
end

circ_check = 0;
rect_check = 0;
% Say what they are.
% IMPORTANT NOTE: depending on the aspect ratio of the rectangle or triangle
for blobNumber = 1 : numberOfObjects
	
	
	% Determine the shape.
	if circularities(blobNumber) < 1.2
		message = sprintf('The circularity of object #%d is %.3f,\nso the object is a circle',...
			blobNumber, circularities(blobNumber));
		shape = 'circle';
        circ_check = 1;
        disp(message);
	elseif circularities(blobNumber) < 1.8 
		message = sprintf('The circularity of object #%d is %.3f,\nso the object is a rectangle',...
			blobNumber, circularities(blobNumber));
		shape = 'rectangle';
        rect_check = 1;
        disp(message);
	
	else
		message = sprintf('The circularity of object #%d is %.3f,\nso the object is something else.',...
			blobNumber, circularities(blobNumber));
		shape = 'something else';
        disp(message);
	end

	% Display in overlay above the object
	overlayMessage = sprintf('Object #%d = %s\ncirc = %.2f, s = %.2f', ...
		blobNumber, shape, circularities(blobNumber), solidities(blobNumber));
	text(blobMeasurements(blobNumber).Centroid(1), blobMeasurements(blobNumber).Centroid(2), ...
		overlayMessage, 'Color', 'r');
	button = questdlg(message, 'Continue', 'Continue', 'Cancel', 'Continue');
	if strcmp(button, 'Cancel')
		break;
    end
     
end

% Displaying final result in console to check for the detected vehicle
disp('------------------Final Result--------------------------------')
    if circ_check == 1 && rect_check == 1
        disp('Image contains both circle and rectangle')
    elseif circ_check == 1
        disp('Image contains only circle')
    elseif rect_check == 1
        disp('Image contains only rectangle')
    else
        disp('Image does not contain any circle or rectangle')
    end
   
