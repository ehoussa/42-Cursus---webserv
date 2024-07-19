<?php
$cookie_name = "user_email";

if (isset($_GET["email"])) {
    $user_email = $_GET["email"];

    // Set the cookie with a 30-day expiration time
    setcookie($cookie_name, $user_email, time() + (86400 * 30), "/");
} elseif (isset($_COOKIE[$cookie_name])) {
    $user_email = $_COOKIE[$cookie_name];
} else {
    $user_email = ""; // Initialize the variable if cookie doesn't exist
}

?>

<html>
<body>

<?php
if (!empty($user_email)) {
    echo "User email is: " . $user_email;
} else {
    echo '<form>
        <label for="email">Enter your email:</label>
        <input type="email" id="email" name="email" required>
        <button type="submit">Submit</button>
    </form>';
}
?>

</body>
</html>