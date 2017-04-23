<?php

$db = new \PDO('sqlite:/var/sqlite/ld38_hst.sqlite3');
$db->setAttribute(\PDO::ATTR_ERRMODE, \PDO::ERRMODE_EXCEPTION);

$db->query("CREATE TABLE IF NOT EXISTS Scores (Name TEXT, IPAddress TEXT, Map TEXT, Car INTEGER, RunLength INTEGER, EnteredOn DATETIME);");

if (!empty($_GET['insert']))
{
    $_GET['map'] = preg_replace('/[^A-Za-z0-9]/', '', $_GET['map']);
    $stmt = $db->prepare("INSERT INTO Scores VALUES (NULL, ?, ?, ?, ?, ?);");
    $stmt->execute([$_SERVER['REMOTE_ADDR'], substr($_GET['map'], 0, 50), (int)$_GET['car'], (int)$_GET['time1'], date('c')]);
}

if (isset($_GET['map']))
{
    $stmt = $db->prepare("SELECT Car, RunLength FROM Scores WHERE Map = ? ORDER BY RunLength ASC LIMIT 20");
    $stmt->execute([$_GET['map']]);
}
else
{
    $stmt = $db->prepare("SELECT Car, RunLength FROM Scores ORDER BY RunLength ASC LIMIT 20");
}

$carMapping = [
    'Catfish',
    'Rocket Sled',
    'Jetcrane',
];

while($row = $stmt->fetch(\PDO::FETCH_ASSOC))
{
    if (isset($carMapping[$row['Car']]))
    {
        $row['Car'] = $carMapping[$row['Car']];

        echo implode("\t", $row) . "\n";
    }
}

