
function openDevTools() {
    alert('Opening dev tools');
    
    window.cefQuery({
        request: 'openDevTools',
        onSuccess: function (response) {
            alert(response);
        },
        onFailure: function(error){
            alert(error);
        }
    });
}