$(document).ready(function() {
     
    //if submit button is clicked
    $(".acceptupdates").click(function () {       
         
        //Get the data from all the fields
        var action = $('input[name=action]');
        var system_id = $('input[name=system_id]');
	var package_name = $('input[name=package_name]');
 
        //Simple validation to make sure user entered something
        //If error found, add hightlight class to the text field
        /*if (name.val()=='') {
            name.addClass('hightlight');
            return false;
        } else name.removeClass('hightlight');
         
        if (email.val()=='') {
            email.addClass('hightlight');
            return false;
        } else email.removeClass('hightlight');
         
        if (comment.val()=='') {
            comment.addClass('hightlight');
            return false;
        } else comment.removeClass('hightlight');
        */ 
        //organize the data properly
        var data = 'action=' + action.val() + '&system_id=' + system_id.val() + '&package_name=' + package_name.val() + '&ajax=true';
         
        //disabled all the text fields
        //$('.text').attr('disabled','true');
         
        //show the loading sign
        //$('.loading').show();
         alert(data);
        //start the ajax
        $.ajax({
            //this is the php file that processes the data and send mail
            url: "mark-accepted-updates.php",
             
            //GET method is used
            type: "POST",
 
            //pass the data        
            data: data,    
             
            //Do not cache the page
            cache: false,
             
            //success
            success: function (html) 
	    {
                //if process.php returned 1/true (send mail success)
                if (html==1) 
		{
                    //hide the form
                    //$('.form').fadeOut('slow');                
                     
                    //show the success message
                    //$('.done').fadeIn('slow');
			//alert("Success");
			//(".acceptupdates").hide();
			//$(this).hide();
                     
                //if process.php returned 0/false (send mail failed)
                }
		else
		{
			alert('Sorry, unexpected error. Please try again later.');
		}
            }
        });
         
        //cancel the submit button default behaviours
        return false;
	//event.preventDefault();
    });
}); 
