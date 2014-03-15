// Copyright © 2009-2011, Rob Chandler.
// Please don't use this file without purchasing FAR. http://helpware.net/FAR/
// This effectively licenses you to use my code. This code (before modification) was 
// first written by Jean-Claude Manoli.
// Changes
// RWC: 2005-04-01 - Fix image pre-load section. Last line had typo causing some images not to load
// RWC: 2005-05-21 - Some work fixing TOC Sync
// RWC: 2008-01-30 - Change resizeTree() to be compatible with non-MS browsers
// RWC: 2009-06-10 - All files now saved in UTF-8 file format.
// RWC: 2009-09-26 - Allow Opera browser to scroll the tree when syncing TOC.
// RWC: 2011-09-10 - Fix Sync for \\server\ UNC paths.
// RWC: 2011-09-11 - Fix Sync for CJK paths.
// RWC: 2012-09-04 - Added selectNext(fwd) & findLinkNode()


/* Original Copyright © 2002 Jean-Claude Manoli [jc@manoli.net]
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the author(s) be held liable for any damages arising from
 * the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 *   1. The origin of this software must not be misrepresented; you must not
 *      claim that you wrote the original software. If you use this software
 *      in a product, an acknowledgment in the product documentation would be
 *      appreciated but is not required.
 * 
 *   2. Altered source versions must be plainly marked as such, and must not
 *      be misrepresented as being the original software.
 * 
 *   3. This notice may not be removed or altered from any source distribution.
 */ 



var treeSelected = null; //last treeNode clicked

//pre-load tree nodes images
var imgPlus = new Image();
imgPlus.src="treenodeplus.gif";
var imgMinus = new Image();
imgMinus.src="treenodeminus.gif";
var imgDot = new Image();
imgDot.src="treenodedot.gif";    //rwc - fixed. Was... imgPlus.src="treenodedot.gif";


function findNode(el)
{
// Takes element and determines if it is a treeNode.
// If not, seeks a treeNode in its parents.
	while (el != null)
	{
		if (el.className == "treeNode")
		{
			break;
		}
		else
		{
			el = el.parentNode;
		}
	}
	return el;
}


function clickAnchor(el)
{
// handles click on a TOC link
//
	expandNode(el.parentNode);
	selectNode(el.parentNode);
	el.blur();
}

function findLinkNode(node)
{
	if (node == null || node == undefined)
		node = treeSelected;
	node = findNode(node);
	if (node == null)  
		return null;
	var anchors = node.getElementsByTagName('A');
	if (anchors.length > 0)
		return anchors[0];
	return null;
}


function selectNext(fwd)
{
// Sync forward or back from current selected. Return href of newly selected node.
//
	var el;
	var aref = "";
        var node = document.getElementById('treeRoot');
	var anchors = node.getElementsByTagName('A');

	//nothing selected? - Select the first node
	if (treeSelected == null)  
	{
		if (anchors.length > 0 && anchors[0] != null && anchors[0] != undefined)
		{
			el = anchors[0];
			selectAndShowNode(el);
			aref = el.getAttribute('href');
		}
	}
	else //select the next node
	{
		for(var i = 0; i < anchors.length; i++)
		{
			el = anchors[i];
			if (findNode(el) == treeSelected)  // find the current selected node & walk fwd or back
			{
				if (fwd) el = anchors[i+1];
				else     el = anchors[i-1];
				if (el != null && el != undefined)
				{
					selectAndShowNode(el);	
					aref = el.getAttribute('href');
				}
				break;		
			}
		}
	}
	return aref;
}


function selectNode(el)
{
// Un-selects currently selected node, if any, and selects the specified node
//
	if (treeSelected != null)
	{
		setSubNodeClass(treeSelected, 'A', 'treeUnselected');
	}
	setSubNodeClass(el, 'A', 'treeSelected');
	treeSelected = el;
}


function setSubNodeClass(el, nodeName, className)
{
// Sets the specified class name on el's first child that is a nodeName element
//
	var child;
	for (var i=0; i < el.childNodes.length; i++)
	{
		child = el.childNodes[i];
		if (child.nodeName == nodeName)
		{
			child.className = className;
			break;
		}
	}
}


function expandCollapse(el)
{
//	If source treeNode has child nodes, expand or collapse view of treeNode
//
	if (el == null)
		return;	//Do nothing if it isn't a treeNode
		
	var child;
	var imgEl;
	for(var i=0; i < el.childNodes.length; i++)
	{
		child = el.childNodes[i];
		if (child.src)
		{
			imgEl = child;
		}
		else if (child.className == "treeSubnodesHidden")
		{
			child.className = "treeSubnodes";
			imgEl.src = "treenodeminus.gif";
			break;
		}
		else if (child.className == "treeSubnodes")
		{
			child.className = "treeSubnodesHidden";
			imgEl.src = "treenodeplus.gif";
			break;
		}
	}
}


function expandNode(el)
{
//	If source treeNode has child nodes, expand it
//
	var child;
	var imgEl;
	for(var i=0; i < el.childNodes.length; i++)
	{
		child = el.childNodes[i];
		if (child.src)
		{
			imgEl = child;
		}
		if (child.className == "treeSubnodesHidden")
		{
			child.className = "treeSubnodes";
			imgEl.src = "treenodeminus.gif";
			break;
		}
	}
}

function GetUnixPath(url)
{
	var path = url.replace(/\\/g, '/');         // DOS to Unix slash
	path = path.replace(/\/\/\//, "//");        // Force 2 slashes xxx://xxx
	path = path.replace(/\/\/\//, "//");
	path = path.replace(/\/\/\//, "//");
	path = path.replace(/\/\/\//, "//");
	return path;
}


function syncTree(href)
{
// Selects and scrolls into view the node that references the specified URL
//
        //RWC 2005-05-21 - This is the real URL base of the TOC
        var gbase = GetUnixPath(location.href);
        gbase = decodeURI(gbase);
        gbase = gbase.substr(0, gbase.lastIndexOf('/') + 1);   //trim off file name. Leave trailing /

	var loc = new String();
	loc = GetUnixPath(href);

	loc = encodeURI(loc); //encode as valid URI
        //RWC 2005-05-21 - properly Scrub URL of encoding
        loc = decodeURI(loc);  //Converts %2520 -> %20  (under FireFox)
        loc = decodeURI(loc);  //Converts %20 = ' '

	var tocEl = findHref(document.getElementById('treeRoot'), loc, gbase);
	if (tocEl != null)
	{
		selectAndShowNode(tocEl);
	}
}

function findHref(node, href, base)
{
// find the <a> element with the specified href value
//
        //RWC 24/3/2006: Consider any bookmark on the URL to test
        var href_BaseURL = '';
        var iBookmark = href.indexOf('#');
        if (iBookmark > 0)
          href_BaseURL = href.substr(0, iBookmark);


	var el;
	var anchors = node.getElementsByTagName('A');
	for (var i = 0; i < anchors.length; i++)
	{
		el = anchors[i];
		var aref = new String();
		aref = el.getAttribute('href');
		
		if ((aref.substring(0, 7) != 'http://') 
		&& (aref.substring(0, 8) != 'https://')
		&& (aref.substring(0, 7) != 'file://'))
		{
			aref = base + aref;
		}

		aref = GetUnixPath(decodeURI(aref));
                //if (i < 5)
	        //  alert('aref=' + aref + ', href=' + href + ', base=' + base);

                //RWC: If href has #bookmark and aref does not then compare without bookmarks
                if ((href_BaseURL.length > 0) && (aref.indexOf('#') < 0))
                  if (aref == href_BaseURL)
                     return el;

		if (aref == href)
		{
			return el;
		}
	}
	return null;
}

function selectAndShowNode(node)
{
// Selects and scrolls into view the specified node
//
	var el = findNode(node);
	if (el != null) 
	{
		selectNode(el);
		do 
		{
			expandNode(el);
			el = findNode(el.parentNode);
		} while ((el != null))  
		
		//vertical scroll element into view
		var windowTop;
		var windowBottom;
		var treeDiv = document.getElementById('tree');
		
		var ua = window.navigator.userAgent.toLowerCase();
		if ((i = ua.indexOf('msie')) != -1)
		{
			windowTop = node.offsetTop - treeDiv.scrollTop;
			windowBottom = treeDiv.clientHeight - windowTop - node.offsetHeight;
		}
		else if (ua.indexOf('gecko') != -1)
		{
			windowTop = node.offsetTop - treeDiv.offsetTop - treeDiv.scrollTop;
			windowBottom = treeDiv.clientHeight - windowTop - node.offsetHeight;
		}
		else if (ua.indexOf('opera') != -1)
		{
			windowTop = node.offsetTop - treeDiv.offsetTop - treeDiv.scrollTop;
			windowBottom = treeDiv.clientHeight - windowTop - node.offsetHeight;
		}
		else 
		{
			return;
		}
		
		if (windowTop < 0)
		{
			treeDiv.scrollTop += windowTop - 18;
			return;
		}
		if (windowBottom < 0)
		{
			treeDiv.scrollTop -= windowBottom - 18;
			return;
		}
	}
}



function GetFrameWidth() 
{ 
	var x = 300; 
	if (self.innerHeight) // all except Explorer 
   	    x = self.innerWidth; 
	else if (document.documentElement && document.documentElement.clientHeight) // Explorer 6 Strict Mode 
	    x = document.documentElement.clientWidth; 
	else if (document.body) // other Explorers 
	    x = document.body.clientWidth; 
	return(x); 
} 

function GetFrameHeight() 
{ 
	var y = 400; 
	if (self.innerHeight) // all except Explorer 
	    y = self.innerHeight; 
	else if (document.documentElement && document.documentElement.clientWidth) // Explorer 6 Strict Mode 
	    y = document.documentElement.clientHeight; 
	else if (document.body) // other Explorers 
	    y = document.body.clientHeight; 
	return(y); 
} 

function resizeTree() 
{
	var treeDiv = document.getElementById("tree");
	var DivFooter = document.getElementById("DivFooter");
              var xTop = treeDiv.offsetTop;
	if ((DivFooter != null) && (DivFooter != undefined))
	    xTop = xTop + DivFooter.offsetHeight;
	treeDiv.style.width = GetFrameWidth();
	var HH = GetFrameHeight(); 
	if (HH -  xTop > 0)
	    treeDiv.style.height = HH -  xTop;
}

// old original func
//function resizeTree()
//{
//	var treeDiv = document.getElementById('tree');
//	//treeDiv.setAttribute('style', 'width: ' + document.body.offsetWidth + 'px; height: ' + (document.body.offsetHeight - 27) + 'px;');
//	treeDiv.style.width = document.documentElement.offsetWidth;
//	treeDiv.style.height = document.documentElement.offsetHeight - 27;
//}
