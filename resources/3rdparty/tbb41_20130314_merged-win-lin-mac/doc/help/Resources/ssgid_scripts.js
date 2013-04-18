var popupWindow = null;
function popUp(URL) {
    popupWindow = window.open(URL,'name','titlebar=no,toolbar=no,status=no,location=no,menubar=no,resizable=yes,scrollbars=yes,height=550,width=500');
  
   if(popupWindow && window.focus) {
      popupWindow.focus();
    }

    var el = window.event;
    el.returnValue = false;
    el.cancelBubble = true;
}
var day = null;
var id = null;
function TestpopUp(URL) {
day = new Date();
id = day.getTime();
eval("page" + id + " = window.open(URL, '" + id + "', 'toolbar=0,scrollbars=1,location=0,statusbar=0,menubar=0,resizable=1,width=500,height=550,left = 470');");
}
