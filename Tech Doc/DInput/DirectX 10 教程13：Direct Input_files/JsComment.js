$(function () {
    var articleId = GetQueryString("articleId");
    if (articleId != null)
        $("#articleId").attr("value", articleId);   

    CKEDITOR.replace('subContent',{ customConfig: 'my_config.js' });

    $("#refreshimg").click(function () {
        $("#refreshimg").attr("src", "Handler/CreateCheckCodeImage.ashx?" + new Date);
    });    
    // 中文的验证
    $.validator.addMethod("chinese", function (value, element) {
        var chinese = /[\u4e00-\u9fa5]+/;
        return this.optional(element) || (chinese.test(value));
    }, "必须包含中文");

    $("#subForm").validate({
        submitHandler: function (form) {
            $(form).ajaxSubmit({
                beforeSend: function () {
                    $('<div class="updating">留言上传中...</div>').insertBefore($("#submit"));
                },
                complete: function () {
                    $(".updating").fadeOut('slow', function () {
                        $(this).remove();
                    });
                    // 刷新验证码
                    $("#refreshimg").triggerHandler("click");
                    //刷新留言
                    RefreshComment()
                },
                contentType: "application/x-www-form-urlencoded; charset=utf-8",
                resetForm: true
            });
            CKEDITOR.instances.subContent.setData("");
        },
        ignore: [],
        rules: {
            author: {
                required: true,
                rangelength: [2, 20]
            },
            subContent: {
                required: function()
                {
                    CKEDITOR.instances.subContent.updateElement();
                },
                //required: true,
                rangelength: [2, 3000],
                chinese: true
            },            
            captcha: {
                required: true,
                remote: {
                    url: "Handler/ValidateHandler.ashx",
                    contentType: "application/x-www-form-urlencoded; charset=utf-8",
                    type: "post",
                    data: {
                        captcha: function () {
                            return $("#captcha").val();
                        }
                    }
                }
            }
        },
        messages: {
            author: {
                required: "请输入姓名",
                rangelength: "请输入一个长度介于2和20之间的字符串"
            },
            subContent:
            {
                required: "留言内容不能为空",
                chinese: "必须包含中文",
                rangelength: "请输入一个长度介于2和3000之间的字符串"
            },
            captcha: {
                required: "请输入验证码",
                remote: "验证码错误"
            }
        }
    });
});

function RefreshComment()
{
    var articleId = GetQueryString("articleId");
    $.ajax({
        url: "Handler/CommentGetHandler.ashx",
        dataType: "json",
        data: "articleId=" + articleId,
        beforeSend: function () {
            $('<div class="updating" style="margin-left:18px; margin-top:38px;">数据加载中...</div>').insertBefore($("#CommentWrap"));
        },
        success: function (data) {
            $("#CommentWrap").empty();
            $.each(data, function (i) {
                var date = jsonDateParser(data[i].addTime);
                $("#CommentWrap").append("<div class='popover right comment' style='width: 880px'><div class='arrow'></div><h4 class='popover-title'><span class='label label-info'>" + data[i].author + "</span><span class='post_date'>" + date + "</span></h4><div class='popover-content'>" + data[i].content + "</div></div>");
            });
        },
        complete: function () {
            $(".updating").fadeOut('slow', function () {
                $(this).remove();
            });
        }
    });
}

function jsonDateParser(value) {
    if (typeof value === 'string') {
        if (/\/Date\((\d+)(?:([+-])(\d\d)(\d\d))?\)\//ig.test(value)) {
            var d = new Date(parseInt(RegExp.$1, 10) - (3600000 * 8));
            if (!!RegExp.$3) {
                d.setHours(d.getHours() + parseInt(RegExp.$2 + RegExp.$3, 10));
            }
            if (!!RegExp.$4) {
                d.setMinutes(d.getMinutes() + parseInt(RegExp.$2 + RegExp.$4, 10));
            }
            return d;
        }
    }
    return value;
}

