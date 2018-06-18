
function openDevTools() {
    window.executeQuery({
        request: 'openDevTools',
        onSuccess: function (response) {
            console.log(response);
        },
        onFailure: function(error, message){
            console.log("Error Code %d - %s", error, message);
        }
    });
}