-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- 主機： 127.0.0.1
-- 產生時間： 2026-04-21 06:23:09
-- 伺服器版本： 10.4.32-MariaDB
-- PHP 版本： 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- 資料庫： `taclass`
--

-- --------------------------------------------------------

--
-- 資料表結構 `accounts`
--

CREATE TABLE `accounts` (
  `acc_id` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `acc_name` varchar(50) NOT NULL,
  `balance` decimal(15,2) DEFAULT 0.00
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- 傾印資料表的資料 `accounts`
--

INSERT INTO `accounts` (`acc_id`, `user_id`, `acc_name`, `balance`) VALUES
(1, 1, '大明的遊局帳號', 5000.00),
(2, 1, '大明的台心帳號', 2000.00),
(3, 2, '小華的遊局帳號', 45000.00);

-- --------------------------------------------------------

--
-- 資料表結構 `categories`
--

CREATE TABLE `categories` (
  `cat_id` int(11) NOT NULL,
  `cat_name` varchar(50) NOT NULL,
  `budget_limit` decimal(15,2) DEFAULT 0.00
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- 傾印資料表的資料 `categories`
--

INSERT INTO `categories` (`cat_id`, `cat_name`, `budget_limit`) VALUES
(1, '飲食', 15000.00),
(2, '交通', 3000.00),
(3, '娛樂', 5000.00),
(4, '購物', 10000.00),
(5, '其他', 2000.00);

-- --------------------------------------------------------

--
-- 資料表結構 `currencies`
--

CREATE TABLE `currencies` (
  `curr_id` int(11) NOT NULL,
  `code` char(3) NOT NULL,
  `exchange_rate` decimal(10,4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- 傾印資料表的資料 `currencies`
--

INSERT INTO `currencies` (`curr_id`, `code`, `exchange_rate`) VALUES
(1, 'TWD', 1.0000),
(2, 'USD', 32.1500),
(3, 'JPY', 0.2130);

-- --------------------------------------------------------

--
-- 資料表結構 `transactions`
--

CREATE TABLE `transactions` (
  `t_id` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `acc_id` int(11) NOT NULL,
  `curr_id` int(11) NOT NULL,
  `cat_id` int(11) NOT NULL,
  `amount` decimal(15,2) NOT NULL,
  `description` text DEFAULT NULL,
  `created_at` datetime DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- 傾印資料表的資料 `transactions`
--

INSERT INTO `transactions` (`t_id`, `user_id`, `acc_id`, `curr_id`, `cat_id`, `amount`, `description`, `created_at`) VALUES
(1, 1, 1, 1, 1, 120.00, '午餐-排骨飯', '2026-03-30 12:30:00'),
(2, 1, 1, 1, 2, 50.00, '搭公車', '2026-03-30 13:00:00'),
(3, 1, 2, 2, 4, 25.00, 'Amazon 買電子書 (USD)', '2026-03-31 10:00:00'),
(4, 2, 3, 1, 1, 450.00, '跟朋友吃義大利麵', '2026-04-01 18:00:00'),
(5, 2, 3, 3, 3, 5000.00, '東京淺草寺御守 (JPY)', '2026-04-07 14:26:36'),
(6, 2, 3, 2, 4, 15.00, '訂閱 ChatGPT Plus (USD)', '2026-04-07 14:26:36');

-- --------------------------------------------------------

--
-- 資料表結構 `user1`
--

CREATE TABLE `user1` (
  `user_id` int(11) NOT NULL,
  `user_name` varchar(50) NOT NULL,
  `user_password` varchar(255) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- 傾印資料表的資料 `user1`
--

INSERT INTO `user1` (`user_id`, `user_name`, `user_password`) VALUES
(1, '王大明', ''),
(2, '李小華', '');

--
-- 已傾印資料表的索引
--

--
-- 資料表索引 `accounts`
--
ALTER TABLE `accounts`
  ADD PRIMARY KEY (`acc_id`),
  ADD KEY `user_id` (`user_id`);

--
-- 資料表索引 `categories`
--
ALTER TABLE `categories`
  ADD PRIMARY KEY (`cat_id`);

--
-- 資料表索引 `currencies`
--
ALTER TABLE `currencies`
  ADD PRIMARY KEY (`curr_id`),
  ADD UNIQUE KEY `code` (`code`);

--
-- 資料表索引 `transactions`
--
ALTER TABLE `transactions`
  ADD PRIMARY KEY (`t_id`),
  ADD KEY `user_id` (`user_id`),
  ADD KEY `acc_id` (`acc_id`),
  ADD KEY `curr_id` (`curr_id`),
  ADD KEY `cat_id` (`cat_id`);

--
-- 資料表索引 `user1`
--
ALTER TABLE `user1`
  ADD PRIMARY KEY (`user_id`);

--
-- 在傾印的資料表使用自動遞增(AUTO_INCREMENT)
--

--
-- 使用資料表自動遞增(AUTO_INCREMENT) `accounts`
--
ALTER TABLE `accounts`
  MODIFY `acc_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=7;

--
-- 使用資料表自動遞增(AUTO_INCREMENT) `categories`
--
ALTER TABLE `categories`
  MODIFY `cat_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;

--
-- 使用資料表自動遞增(AUTO_INCREMENT) `currencies`
--
ALTER TABLE `currencies`
  MODIFY `curr_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- 使用資料表自動遞增(AUTO_INCREMENT) `transactions`
--
ALTER TABLE `transactions`
  MODIFY `t_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=15;

--
-- 使用資料表自動遞增(AUTO_INCREMENT) `user1`
--
ALTER TABLE `user1`
  MODIFY `user_id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;

--
-- 已傾印資料表的限制式
--

--
-- 資料表的限制式 `accounts`
--
ALTER TABLE `accounts`
  ADD CONSTRAINT `accounts_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `user1` (`user_id`) ON DELETE CASCADE;

--
-- 資料表的限制式 `transactions`
--
ALTER TABLE `transactions`
  ADD CONSTRAINT `transactions_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `user1` (`user_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `transactions_ibfk_2` FOREIGN KEY (`acc_id`) REFERENCES `accounts` (`acc_id`) ON DELETE CASCADE,
  ADD CONSTRAINT `transactions_ibfk_3` FOREIGN KEY (`curr_id`) REFERENCES `currencies` (`curr_id`),
  ADD CONSTRAINT `transactions_ibfk_4` FOREIGN KEY (`cat_id`) REFERENCES `categories` (`cat_id`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
