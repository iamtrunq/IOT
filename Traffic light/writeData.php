<?php
    // log in vao database
    include("config.php");
    // doc user input
    $id = $_POST["group"];
    $r = $_POST["r"];
    $y = $_POST["y"];
    $g = $_POST["g"];

    // update lai database
    $sql = "update traffic_lights set red = $r, yellow = $y, green = $g where id = $id";
    mysqli_query($conn, $sql);

    mysqli_close($conn);

?>