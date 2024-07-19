<?php

while (1);

//  is set  => wach kayn
if (isset($_GET['name'])) {
    // Retrieve and sanitize the 'name' parameter
    $name = htmlspecialchars($_GET['name']);
    
    // Print the value of the 'name' parameter
    echo "Hello, $name!";
} else {
    echo "Please provide a 'name' parameter.";
}
?>