//======================================================
// ALIGN v 1.0.3   DialogAlign .02
//
// Eagle v6.0 or later is required. Only the board editor is supported.
//
// Aligns board components in group or listed at the command line
// The direction of alignment is autodetected. The components are aligned
// after the origins of the top or leftmost component
//
// This software is released under the BSD 4-clause license:
//
// Copyright (c) 2010-2011, Damhave Systems
// All rights reserved.

// Modified 2010-05-21 LJW - Correct group coordinate determination & quote output script name on exit
// Modified 2013-06-13 gh (IPG Automotive GmbH) - Adaptations to Eagle 6.x
// Modified 2013-06-13 gh (IPG Automotive GmbH) - Original Part order is now kept, independant from Part Name!

// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

//	*	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//	*	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer
// 		in the documentation and/or other materials provided with the distribution.
//	*	Neither the name of the Damhave Systems nor the names of its contributors may be used to endorse or promote products derived from
//	    this software without specific prior written permission.
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
// BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
// OF SUCH DAMAGE.
//======================================================
#usage	"<B>Aligns board components in group or listed at the command line</B>"
"<P>"
	"<P>"
	"Align components along a line parallel to the x or y axis of the board.<br>"
	"The direction of alignment is autodetected by looking at the position of the components. The alignment direction can also be forced.<br><br>"
	"<b>Usage:</b>"
	"<blockquote>run align<br>"
	"run align [-flag{flag}] [increment] [ref object] {component}</blockquote>"
	"<b>Flags:</b>"
	"<blockquote>c: align to the bottommost/leftmost Component in group or list<br>"
	"d: Distribute components evenly between the outermost components<br>"
	"e: use regular Expressions (regex) instead of wildcards<br>"
	"g: align to Grid<br>"
	"h: Force alignment on a horizontal line<br>"
	"i: Increment position for each component by the specified value<br>"
	"o: align to reference Object<br>"
	"s: Snap to grid<br>"
	"k: Keep original part order if parts are already in the correct order but not aligned<br>"
	"v: Force alignment on a Vertical line</blockquote>"
	"<I>The components in the selection group are used when no components are specified at the command line.<br>"
	"The align to the bottommost/leftmost component flag -c is default set if no -g or -o flags are specified</I></blockquote>"
	"<P>"
	"<b>Examples:</b><br>"
	"Align the selected components to the bottommost leftmost component, force alignment on a vertical line:"
	"<blockquote>run align -v</blockquote>"
	"Align the resistors R1, R2, R3 on a horizontal line and snap to grid:"
	"<blockquote>run align -hg R1 R2 R3</blockquote>"
	"Align the connectors J1, J2 and J4 to the hole H2:"
	"<blockquote>run align -o H2 J1 J2 J4</blockquote>"
	"Align the selected components to the bottommost leftmost component, increment the position with 0.5 for each component:"
	"<blockquote>run align -i 0.5</blockquote>"
	"Align all LEDs to the nearest grid line and distribute them evenly along the line:"
	"<blockquote>run align -gd LED*</blockquote>"
	"Align all LEDs to the nearest grid line and snap to the grid points along the line:"
	"<blockquote>run align -gs LED*</blockquote>"
	"<P>"
	"Copyright (c) 2010-2011 Damhave Systems"
	"<P>"
	"<Author>Danny Damhave<br>Damhave Systems<br>www.damhave.com</Author><br><br>"
	"This ulp is released under the BSD 4-clause license. See source code for details!"

enum {false, true};
enum {ALIGN_ALONG_YAXIS, ALIGN_ALONG_XAXIS};
enum {GROUP, LIST};
string outputFileName = "aligntempfile.scr";
string partlist[];
int alignment, xRefObject = INT_MAX, yRefObject = INT_MAX, xAligned = INT_MAX, yAligned = INT_MAX, xMin = INT_MAX, xMax = INT_MIN, yMax = INT_MIN, yMin = INT_MAX, noElements = 0, mode = GROUP, firstInList = 1,
	alignToComponentFlag = true, keepFlag=false, alignToGridFlag = false, alignToObjectFlag = false, alignOnVerticalLine = false, alignOnHorizontalLine = false, regexFlag = false, snapFlag = false, distributeFlag = false, incrementFlag = false;
real distance, increment, xAvg = 0.0, yAvg = 0.0;

int Result = 0;
// this is a file where param values are stored - it would be better in static memory but
// that is not available to ULPs at this time
string ULPfilename = path_ulp[0] + '/' + "DialogAlignULP.txt"; 

// Checkboxes variables for the dialog
int horizontal = 0, vertical = 0, dlgIncrement  = 0, distribute = 0, snapGrid  = 0, alignToGrid = 0, align = , keepInOrder = 0;
int dlgRegEx = 0, alignToObject = 0, alignComponents = 0, flagsSet = 0, numberOfComponents, IncrementAmtInMils = 0;

int DBbools[];
string ObjectString = "";
string ComponentText = "Components sep. by spaces";
string ComponentList[];  // two dim text array (list) 
string boolParams = "000000000000000000";
//string compParams = "Components sep. by spaces";
string incrmntParm = "000000";
string str_DBbools= "000000000000";
string inString;


// Align ULP 1.0.3 by Danny Damhave Damhave Systems www.damhave.com
// Dialog box implementation by Paul Badger 2014

void initParamFile(){
int size = filesize(ULPfilename);
if (!size){output(ULPfilename,"wt"){
      printf("0000000000000000000"); 

void saveParams(){       // save configs
DBbools[0] = horizontal;
DBbools[1] = vertical;
DBbools[2] = snapGrid;
DBbools[3] = alignToGrid;
DBbools[4] = keepInOrder;
DBbools[5] = alignToObject;
DBbools[6] = alignComponents;
DBbools[7] = dlgRegEx;
DBbools[8] = dlgIncrement;
DBbools[9] = distribute;
DBbools[10] = 0; //unused
DBbools[11] = 0; //unused

for (int i = 0; i < 12; i++){
      if  (DBbools[i]) str_DBbools[i] = '1';
      else str_DBbools[i] = '0';
 }

output(ULPfilename,"wt"){
      printf("Bools:%s\n", str_DBbools);
      printf("ObStr:%s\n", ObjectString);
      printf("CmpSt:%s\n", ComponentText);
      printf("IncMs:%d\n", IncrementAmtInMils);
    }

}


void getParams(){
int i = 0;
string Sarray[];
int nStrings = fileread(inString, ULPfilename);
nStrings = strsplit(Sarray, inString, '\n');

ObjectString = strsub(Sarray[1], 6, strlen(Sarray[1]));
ComponentText = strsub(Sarray[2], 6, strlen(Sarray[2]));
IncrementAmtInMils = strtol(strsub(Sarray[3], 6, strlen(Sarray[2])));


//Parse out Boolean checkbox info

horizontal =        Sarray[0][i++ + 6] - '0'; 
vertical =          Sarray[0][i++ + 6] - '0'; 
snapGrid =          Sarray[0][i++ + 6] - '0';
alignToGrid =       Sarray[0][i++ + 6] - '0';
keepInOrder =       Sarray[0][i++ + 6] - '0';
alignToObject =     Sarray[0][i++ + 6] - '0'; 
alignComponents =   Sarray[0][i++ + 6] - '0'; 
dlgRegEx =          Sarray[0][i++ + 6] - '0'; 
dlgIncrement =      Sarray[0][i++ + 6] - '0'; 
distribute =        Sarray[0][i++ + 6] - '0';
DBbools[9] =        Sarray[0][i++ + 6] - '0';
DBbools[10] =       Sarray[0][i++ + 6] - '0';
DBbools[11] =       Sarray[0][i++ + 6] - '0';  

}


getParams();
Result = dlgDialog("Dialog for Align ULP") {
  dlgVBoxLayout {
     dlgStretch(0);
     dlgGroup("Align ULP by Danny Damhave v1.0.3") {
        dlgStretch(1);
        dlgGridLayout {
          

           dlgCell(1, 1) {   dlgCheckBox("&Force Vertical -v", vertical) if (vertical) horizontal = 0; }; 
           dlgCell(2, 1) {  dlgCheckBox("&Force Horizontal -h", horizontal) if (horizontal) vertical = 0; };
           dlgCell(3, 1) {  dlgCheckBox("&Align to Grid -s", alignToGrid) if (alignToGrid) alignComponents = 0; };          
           dlgCell(4, 1) {  dlgCheckBox("&Snap to Grid -s", snapGrid);  };
           dlgCell(5, 1) {  dlgCheckBox("&Keep Parts In Order -k", keepInOrder); };
           dlgCell(6, 1) {  dlgCheckBox("&Align To Object -o", alignToObject) if (alignToObject) alignComponents = 0; };
           dlgCell(7, 1) {  dlgStringEdit(ObjectString); }; 
           dlgCell(8, 1) {  dlgCheckBox("&Align Components -c", alignComponents); };
           dlgCell(9, 1) {  dlgStringEdit(ComponentText); };
           dlgCell(10, 1){  dlgCheckBox("&Use Regular Expressions -e", dlgRegEx); };   
           dlgCell(11, 1){  dlgCheckBox("&Distribute -d", distribute) if (distribute) dlgIncrement = 0; };
           dlgCell(12, 1){  dlgCheckBox("&Increment -i (in mils)", dlgIncrement) if (dlgIncrement) distribute = 0; };
           dlgCell(13, 1){  dlgIntEdit(IncrementAmtInMils, -40000, 40000); };
           dlgCell(14, 1){  dlgLabel(ObjectString, 1 ); };
           dlgRedisplay();
         }
           
      }
   
        dlgHBoxLayout {
            dlgPushButton("+OK"){
            saveParams();
            dlgAccept(1);           
            }
           dlgStretch(1);
           dlgSpacing(30);
           dlgStretch(1);
           dlgPushButton("-CANCEL") dlgReject(-1);
           dlgStretch(0);
           }
   
          dlgStretch(0);
          dlgRedisplay();
       }
  };

if (Result == -1){
exit (0);
}

if (Result == 1){  // dialog exited with "OK"
numberOfComponents = strsplit(ComponentList, ComponentText, ' ');   // parse out component list

}


// dialog box ends here

int isMatch(string s, string test, int useRegex)
{
int j;
//dlgMessageBox(s + ' ' + test + j++ );


	string regex = !useRegex ?  "^" : test;
	if (!useRegex)
	{
		for(int i = 0; test[i];)
		{
			switch(test[i])
			{
				case '[': regex += strsub(test, i, strchr(test, ']', i) - i + 1), i += strchr(test, ']', i) - i + 1; break;
				case '?': regex += ".", i++; break;
				case '*': regex += ".*", i++; break;
				case '+': case '^': case '$': case '.': case '{': case '}': case '(': case ')':  case '|': case '\\':
				regex += "\\" + test[i], i++; break;
				default: regex += test[i], i++;
			}
		}
		regex +='$';
	}
	return strxstr(s, regex, 0) == 0;
}



// argv is a list - two-dimensional text array


int inList(string name)
{
//dlgMessageBox(name);
	for(int i = 0 /*firstInList*/; (i < numberOfComponents) && (ComponentList[i] != "")   /* argc && argv[i]  != ""*/ ; i++)
	{
		if (regexFlag)
		{
			if (isMatch(name, ComponentList[i] /*argv[i]*/ , regexFlag)) return true;
		}
		else
		{
			if (isMatch(strupr(name), strupr(ComponentList[i] /*argv[i]*/), regexFlag)){ /*dlgMessageBox("true");*/ return true; }
		}
	}
  //  dlgMessageBox("false");
	return false;
}

int snapToGrid(UL_BOARD brd, real coordinate)
{

	real gSize;
	if (brd.grid.unitdist == GRID_UNIT_MIC) gSize = mic2u(brd.grid.distance);
	else if  (brd.grid.unitdist == GRID_UNIT_MM) gSize = mm2u(brd.grid.distance);
	else if  (brd.grid.unitdist == GRID_UNIT_MIL) gSize = mil2u(brd.grid.distance);
	else if  (brd.grid.unitdist == GRID_UNIT_INCH) gSize = inch2u(brd.grid.distance);
	// printf("# old coordinate=%f gSize=%f brd.grid.distance=%f\n", coordinate, gSize, brd.grid.distance);
	// printf("# new coordinate = %f mm2u=%d\n", (round (coordinate / gSize) * gSize), mm2u(1));
	return (round (coordinate / gSize) * gSize);
}

real input2u(UL_BOARD brd, real inputValue)
{
	real convFactor;
	if (brd.grid.unitdist == GRID_UNIT_MIC) convFactor = mic2u(1);
	else if  (brd.grid.unitdist == GRID_UNIT_MM) convFactor = mm2u(1);
	else if  (brd.grid.unitdist == GRID_UNIT_MIL) convFactor = mil2u(1);
	else if  (brd.grid.unitdist == GRID_UNIT_INCH) convFactor = inch2u(1);
	return (round (inputValue * convFactor));
}



// Liste nach den Koordinatien sortieren, um ursprüngliche Reihenfolge beizubehalten
int SortPartList (UL_BOARD brd, int nEls, int DirIsX, int reverse)
{
    real coord[], c;
    int getauscht = 0;
    string t;
    int i, j;

    if (keepFlag == false) return 0;

    for (i=0; i<nEls; i++)
    {
	brd.elements(e1)
	{
	    if (e1.name == partlist[i]) {
		coord[i] = (DirIsX==1) ? e1.x : e1.y;
	    }
	}
    }

    do {
	getauscht = 0;
	for (i=0; i<nEls-1; i++)
	{
	    j=i+1;

	    if ((reverse == 0 && coord[i] > coord[j]) || (reverse == 1 && coord[i] < coord[j]))
	    {
		getauscht = 1;
		t = partlist[i];
		partlist[i] = partlist[j];
		partlist[j] = t;
		c = coord[i];
		coord[i] = coord[j];
		coord[j] = c;
		// printf ("# Getauscht: %d=%s %d=%s\n", i, partlist[i], j, partlist[j]);
	    }
	}
    } while (getauscht == 1);

    return 0;
}




//		***** The ulp begins here *****
if ((!board && !library )&& !dlgMessageBox("Can only be used in the board editor.\n")) exit(1);
if (EAGLE_VERSION < 6 && !dlgMessageBox("Requires Eagle v5.7 or later.\n")) exit(1);

 // parse out dialog box attributes   //PAB
		if (distribute) distributeFlag = true;
		if (dlgRegEx) regexFlag = true;
		if (horizontal) alignOnHorizontalLine = true;
		if (keepInOrder) keepFlag = true;
		if (dlgIncrement) incrementFlag = true;
		if (alignToGrid) { alignToGridFlag = true; alignToComponentFlag = false; }
        if (alignComponents)  alignToComponentFlag = true;
		if (alignToObject){ alignToObjectFlag = true; alignToComponentFlag = false; }
		if (snapGrid) snapFlag = true;
		if (vertical) alignOnVerticalLine = true;

         // this is kind of clunky - needed to trigger flag parsing below - PAB
         if ( distribute || dlgRegEx || horizontal || keepInOrder || keepInOrder || dlgIncrement || snapGrid || alignComponents || alignToObject || vertical){
          flagsSet = 1; }
                         
                         

if (flagsSet)
{
	//string flags = argv[firstInList];
	//if ((flags[0] == '-') || flagsSet) //PAB
	//{
		//if (strlen(flags) == 1) { dlgMessageBox("No flags.\n"); exit(1); }
		/*for (int i = 1; flags[i]; i++)
		{			
			switch (flags[i])
			{
				case 'c' : alignToComponentFlag = true; break;
				case 'd' : distributeFlag = true; break;
				case 'e' : regexFlag = true; break;
				case 'g' : alignToGridFlag = true; alignToComponentFlag = false; break;
				case 'h' : alignOnHorizontalLine = true; break;
				case 'k' : keepFlag = true; break;
				case 'i' : incrementFlag = true; break;
				case 'o' : alignToObjectFlag = true; alignToComponentFlag = false; break;
				case 's' : snapFlag = true; break;
				case 'v' : alignOnVerticalLine = true; break;
				//default  : dlgMessageBox("No such flag.\n"); exit(3);
			}
		}*/
		firstInList++;  // what does this do? Pointer to flag?
		if (((alignOnHorizontalLine && alignOnVerticalLine) || (incrementFlag && distributeFlag) || (alignToGridFlag && alignToObjectFlag) || (alignToComponentFlag && alignToObjectFlag) || (alignToGridFlag && alignToComponentFlag)) && !dlgMessageBox("Illegal flag combination.\n")) exit(1);
		if (incrementFlag)
		{
			if (incrementFlag  && !(IncrementAmtInMils) && !dlgMessageBox("No increment specified.\n")) exit(1);
			board(brd) { //increment = input2u(brd, strtod(argv[firstInList]));
                          if (IncrementAmtInMils != 0) increment = input2u(brd,real(IncrementAmtInMils)); //PAB
            }
                        
         			firstInList++;
		}
		if (alignToObjectFlag)
		{
			if (alignToObjectFlag /*!(argc>firstInList)*/ && (ObjectString == "")  && !dlgMessageBox("No object specified.\n")) exit(1);
			board(brd) { brd.elements(element) { if (element.name == ObjectString) xRefObject = element.x, yRefObject = element.y; } }
			if ((xRefObject == INT_MAX && yRefObject == INT_MAX) && !dlgMessageBox("No such object.\n")) exit(1);
			firstInList++;
		}
	//}
	else;	// There are no flags therefore use the default align to bottommost/leftmost object in the direction with the most spread
}
if (alignToComponentFlag) mode = LIST;   // (argc>firstInList) mode = LIST;  // don't understand this PAB
else mode = GROUP;

board(brd)
{
    int nEls = 0;
    int i;

    outputFileName = filedir(brd.name)+outputFileName;
    output(outputFileName, "wtD")
    {
	status("Aligning Objects...");
	brd.elements(element)
	{
	    if ((mode == GROUP && ingroup(element)) || (mode == LIST && inList(element.name)))      //  inList(element.name)))  
	    {
		noElements += 1;
		if (element.x < xMin) xMin = element.x;
		if (element.x > xMax) xMax = element.x;
		if (element.y < yMin) yMin = element.y;
		if (element.y > yMax) yMax = element.y;
		xAvg += element.x;
		yAvg += element.y;
		partlist[nEls] = element.name;
		nEls ++;
	    }
	}

	if ((noElements < 2) && !dlgMessageBox("Minimum 2 objects in group/list required.\n")) exit(1);
	xAvg = xAvg / noElements;
	yAvg = yAvg / noElements;

	if (alignOnHorizontalLine) alignment = ALIGN_ALONG_XAXIS;									// Alignment in X direction forced by -h flag ?
	else if (alignOnVerticalLine) alignment = ALIGN_ALONG_YAXIS;								// Alignment in Y direction forced by -v flag ?
	else alignment = ((xAvg - xMin) > (yMax - yAvg)) ? ALIGN_ALONG_XAXIS : ALIGN_ALONG_YAXIS;	// Alignment autodetected on basis of spread

	if (alignment == ALIGN_ALONG_YAXIS)
	{
	    if (alignToObjectFlag) xAligned = xRefObject;											// Align on a vertical line going through the reference object ?
	    else brd.elements(element)																// Align on a vertical line going through to the leftmost bottommost object
	    {
		if ((mode == GROUP && ingroup(element)) || (mode == LIST && inList(element.name)))   //inList(element.name)))
		{
		    if (element.y == yMin) xAligned = (snapFlag || alignToGridFlag) ? snapToGrid(brd, element.x) : ((element.x < xAligned) ? element.x : xAligned);
		}
	    }
	    if (distributeFlag)
		    distance = (yMax - yMin)/(noElements-1);
	    else if (incrementFlag)
		    distance = increment;
	    SortPartList (brd, nEls, 0, 0);
	}
	else
	{
	    if (alignToObjectFlag) yAligned = yRefObject;											// Align on a horizontal line going through the reference object ?
	    else brd.elements(element)																// Align on a horizontal line going through to the leftmost bottommost object
	    {
		if ((mode == GROUP && ingroup(element)) || (mode == LIST && inList(element.name)))               //inList(element.name)))
		{
		    if (element.x == xMin) yAligned = (snapFlag || alignToGridFlag) ? snapToGrid(brd, element.y) : ((element.y < yAligned) ? element.y : yAligned);
		}
	    }
	    if (distributeFlag)
		    distance = (xMax - xMin)/(noElements-1);
	    else if (incrementFlag)
		    distance = increment;
	    SortPartList (brd, nEls, 1, 0);
	}


	int n = 0, totalDistance;
	for (i=0; i<nEls; i++) {
	    brd.elements(element)
	    {
		if (element.name == partlist[i])
		{
		    if (distributeFlag || incrementFlag)
		    {
			    totalDistance = ((alignment == ALIGN_ALONG_YAXIS) ? yMin : xMin) + distance * n;

			    if (alignment == ALIGN_ALONG_YAXIS)
				yAligned = (snapFlag || alignToGridFlag) ? snapToGrid(brd, totalDistance) : totalDistance;
			    else
				xAligned = (snapFlag || alignToGridFlag) ? snapToGrid(brd, totalDistance) : totalDistance;
			    printf("MOVE %s (%fmm %fmm)\n", element.name, u2mm(xAligned), u2mm(yAligned));
			    n++;
		    }
		    else
		    {
			    if (alignment == ALIGN_ALONG_YAXIS)
				printf("MOVE %s (%fmm %fmm)\n", element.name, u2mm(xAligned), u2mm(snapFlag ? snapToGrid(brd, element.y) : element.y));
			    else
				printf("MOVE %s (%fmm %fmm)\n", element.name, u2mm(snapFlag ? snapToGrid(brd, element.x) : element.x), u2mm(yAligned));
		    }
		}
	    }
	}
    }
}
exit("script '" + outputFileName + "'");


