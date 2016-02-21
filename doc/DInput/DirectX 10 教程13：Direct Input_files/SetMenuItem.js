var _hmt = _hmt || [];
(function () {
    var hm = document.createElement("script");
    hm.src = "//hm.baidu.com/hm.js?2d3bf6ce40063546e758a70cd298d21f";
    var s = document.getElementsByTagName("script")[0];
    s.parentNode.insertBefore(hm, s);
})();

$(function () {
    var classId = GetQueryString("classId");
    if (classId == null)
        $("#menuItemDefault").addClass("active");
    else
    {
        $("#nav > li").eq(classId).addClass("active");
    }
});

function GetQueryString(name) {
    var reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)");
    var r = window.location.search.substr(1).match(reg);
    if (r != null) return unescape(r[2]); return null;
}

// 让图片居中的jquery函数
$.fn.imgVAlign = function () {
    return $(this).each(function (i) {
        var bg = $(this).attr('src');
        $(this).parent().css('background', 'url(' + bg + ') no-repeat center center');
        $(this).css('opacity', '0');
    })
}