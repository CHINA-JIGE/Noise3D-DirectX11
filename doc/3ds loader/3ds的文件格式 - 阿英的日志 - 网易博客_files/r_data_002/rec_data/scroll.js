(function($){$.fn.stop=function(){this.get(0).stop&&this.get(0).stop();return this;};$.fn.scroll=function(){this.get(0).scroll&&this.get(0).scroll();return this;};$.fn.jCarouselLite=function(o){o=$.extend({btnPrev:null,btnNext:null,btnGo:null,mouseWheel:false,auto:null,mouseOver:false,speed:200,easing:null,vertical:false,circular:true,visible:3,start:0,scroll:1,beforeStart:null,afterEnd:null},o||{});return this.each(function(){var clearTimer;var running=false,animCss=o.vertical?"top":"left",sizeCss=o.vertical?"height":"width";var div=$(this),ul=$("ul",div),tLi=$("li",ul),tl=tLi.size(),v=o.visible;div.get(0).stop=function(){clearTimer&&clearInterval(clearTimer);};div.get(0).scroll=function(){if(!!clearTimer){clearInterval(clearTimer)}clearTimer=setInterval(function(){go(curr+o.scroll)},o.auto+o.speed);};if(o.isStop){div.attr('isStop',o.isStop);}if(o.circular){ul.prepend(tLi.slice(tl-v-1+1).clone()).append(tLi.slice(0,v).clone());o.start+=v}var li=$("li",ul),itemLength=li.size(),curr=o.start;div.css("visibility","visible");li.css({overflow:"hidden",float:o.vertical?"none":"left"});ul.css({margin:"0",padding:"0",position:"relative","list-style-type":"none","z-index":"1"});div.css({overflow:"hidden",position:"relative","z-index":"2",left:"0px"});var liSize=o.vertical?height(li):width(li);var ulSize=liSize*itemLength;var divSize=liSize*v;li.css({width:li.width(),height:li.height()});ul.css(sizeCss,ulSize+"px").css(animCss,-(curr*liSize));div.css(sizeCss,divSize+"px");if(o.btnPrev)$(o.btnPrev).click(function(){return go(curr-o.scroll)});if(o.btnNext)$(o.btnNext).click(function(){return go(curr+o.scroll)});if(o.btnGo)$.each(o.btnGo,function(i,val){$(val).click(function(){return go(o.circular?o.visible+i:i)})});if(o.mouseWheel&&div.mousewheel)div.mousewheel(function(e,d){return d>0?go(curr-o.scroll):go(curr+o.scroll)});if(o.mouseOver&&o.auto)div.hover(function(){clearInterval(clearTimer)},function(){if(!!clearTimer){clearInterval(clearTimer)};clearTimer=setInterval(function(){go(curr+o.scroll)},o.auto+o.speed)});if(o.auto)if(!!clearTimer){clearInterval(clearTimer)};clearTimer=setInterval(function(){go(curr+o.scroll)},o.auto+o.speed);function vis(){return li.slice(curr).slice(0,v)};function go(to){if(!running){if(o.beforeStart)o.beforeStart.call(this,vis());if(o.circular){if(to<=o.start-v-1){ul.css(animCss,-((itemLength-(v*2))*liSize)+"px");curr=to==o.start-v-1?itemLength-(v*2)-1:itemLength-(v*2)-o.scroll}else if(to>=itemLength-v+1){ul.css(animCss,-((v)*liSize)+"px");curr=to==itemLength-v+1?v+1:v+o.scroll}else curr=to}else{if(to<0||to>itemLength-v)return;else curr=to}running=true;if(o.auto){clearInterval(clearTimer)}ul.animate(animCss=="left"?{left:-(curr*liSize)}:{top:-(curr*liSize)},o.speed,o.easing,function(){if(o.afterEnd)o.afterEnd.call(this,vis());running=false;if(o.auto){if(!!clearTimer){clearInterval(clearTimer)};clearTimer=setInterval(function(){go(curr+o.scroll)},o.auto+o.speed)}});if(!o.circular){$(o.btnPrev+","+o.btnNext).removeClass("disabled");$((curr-o.scroll<0&&o.btnPrev)||(curr+o.scroll>itemLength-v&&o.btnNext)||[]).addClass("disabled")}};return false}})};function css(el,prop){return parseInt($.css(el[0],prop))||0;};function width(el){return el[0].offsetWidth+css(el,'marginLeft')+css(el,'marginRight');};function height(el){return el[0].offsetHeight+css(el,'marginTop')+css(el,'marginBottom');}})(jQuery);

function doColorSwitch(target) {
    window.setInterval(function () {
        $(target + ' .name em').css("background-color", "#1E72EF");
        window.setTimeout(function () {
            $(target + ' .name em').css("background-color", "#F3003F");
        }, 600);
    }, 1200);
}

function dsp1ScrollRun(target, visible, btnNext, btnPrev) {
    $(target).jCarouselLite({
        visible: visible,
        btnNext: btnNext,
        btnPrev: btnPrev,
        scroll: 1,
        auto: 3000,
        easing: 'linear',
        speed: 200,
        mouseOver: true
    });
}

function dsp1ScrollRunN(target, visible, snum, btnNext, btnPrev) {
    $(target).jCarouselLite({
        visible: visible,
        btnNext: btnNext,
        btnPrev: btnPrev,
        scroll: snum,
        auto: 3000,
        easing: 'linear',
        speed: 400,
        mouseOver: true
    });
}
var timerInterval,timerTimeout,timerDelay;
function clearTimerRun() {
	if (!!timerInterval) {
		window.clearInterval(timerInterval);
	}
	if (!!timerTimeout) {
		window.clearTimeout(timerTimeout);
	}
	if (!!timerDelay) {
		window.clearTimeout(timerDelay);
	}
}
function autoAltAd(target, to, ti, to1, mt, mt1) {
	timerInterval = window.setInterval(function() {
		$(target).animate({marginTop:mt});
		$list.stop();
		timerTimeout = window.setTimeout(function() {
			$(target).animate({marginTop:mt1});
			$list.scroll();
		}, to);
	}, ti);
	timerDelay = window.setTimeout(function() {
		$(target).animate({marginTop:mt1});
		$list.scroll();
	}, to1);
};