<?php
header('Content-Type: application/json; charset=utf-8');

$host = 'localhost';
$db   = 'taclass'; 
$user = 'root';
$pass = ''; 
$charset = 'utf8mb4';

$dsn = "mysql:host=$host;dbname=$db;charset=$charset";
$options = [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION, PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_ASSOC];

try { $pdo = new PDO($dsn, $user, $pass, $options); } 
catch (\PDOException $e) { echo json_encode(['error' => $e->getMessage()]); exit; }

$action = $_GET['action'] ?? '';

switch ($action) {
    case 'get_meta':
        $users = $pdo->query("SELECT user_id, user_name FROM user1")->fetchAll();
        $categories = $pdo->query("SELECT cat_id, cat_name FROM categories")->fetchAll();
        $currencies = $pdo->query("SELECT curr_id, code, exchange_rate FROM currencies")->fetchAll();
        echo json_encode(['users' => $users, 'categories' => $categories, 'currencies' => $currencies]);
        break;

    case 'get_user_detail':
        $user_id = $_GET['user_id'];

        $sql_acc = ""; //TODO: 實作二
        $stmt_acc = $pdo->prepare($sql_acc);
        $stmt_acc->execute([$user_id]);
        $accounts = $stmt_acc->fetchAll();

        $sql_trans = "";  // TODO: 實作二 & 實作四
        $stmt_trans = $pdo->prepare($sql_trans);
        $stmt_trans->execute([$user_id]);
        $transactions = $stmt_trans->fetchAll();

        echo json_encode(['accounts' => $accounts, 'transactions' => $transactions]);
        break;

    case 'add_user_with_account':
        try {
            $pdo->beginTransaction();
            // TODO: 實作一
            $pdo->commit();
            echo json_encode(['success' => true]);
        } catch (Exception $e) { $pdo->rollBack(); echo json_encode(['error' => $e->getMessage()]); }
        break;

    case 'add_transaction':
        try {
            $pdo->beginTransaction();
            // TODO: 實作三
            $pdo->commit();
            echo json_encode(['success' => true]);
        } catch (Exception $e) { $pdo->rollBack(); echo json_encode(['error' => $e->getMessage()]); }
        break;
}