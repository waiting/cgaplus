<{load=parts/header.html.tpl}><div class="container-fluid">
    <h3 style="margin-top: 8px;">通行证列表</h3>
    <div class="row"><{loop=accounts account}>
        <div class="col-lg-2 col-md-3 col-sm-4" style="margin:1rem 0;">
            <div class="card">
                <div class="card-body">
                    <h5 class="card-title">{{account.account_name}}</h5>
                    <a class="card-link" onclick="onDeleteAccount($(this).prev().text());">删除</a>
                </div>
            </div>
        </div><{/loop}>
    </div>

</div>
<{load=parts/footer.html.tpl}>
