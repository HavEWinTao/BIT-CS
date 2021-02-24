/*
 Navicat Premium Data Transfer

 Source Server         : MySQL
 Source Server Type    : MySQL
 Source Server Version : 80017
 Source Host           : localhost:3306
 Source Schema         : mmcmail

 Target Server Type    : MySQL
 Target Server Version : 80017
 File Encoding         : 65001

 Date: 16/09/2020 08:35:01
*/

show databases;
create database NeuMail default charset utf8;
use NeuMail;
show tables;

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for tb_mails
-- ----------------------------
DROP TABLE IF EXISTS `tb_mails`;
CREATE TABLE `tb_mails`  (
  `sender` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `recipient` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `Cc_people` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT '-',
  `CS_people` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT '-',
  `file` varchar(50) CHARACTER SET utf8 COLLATE utf8_general_ci NULL DEFAULT '-',
  `time` datetime(0) NULL DEFAULT NULL,
  `title` varchar(50) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `MainText` text CHARACTER SET utf8 COLLATE utf8_general_ci NULL,
  `send_statu` int(11) NULL DEFAULT 0,
  `s_preserve_statu` int(11) NULL DEFAULT 0,
  `r_preserve_statu` int(11) NULL DEFAULT 0,
  `c_preserve_statu` int(11) NULL DEFAULT 0,
  `e_preserve_statu` int(11) NULL DEFAULT 0,
  `r_read_statu` int(11) NULL DEFAULT 0,
  `c_read_statu` int(11) NULL DEFAULT 0,
  `e_read_statu` int(11) NULL DEFAULT 0,
  INDEX `sender`(`sender`) USING BTREE,
  INDEX `recipient`(`recipient`) USING BTREE,
  INDEX `Cc_people`(`Cc_people`) USING BTREE,
  INDEX `CS_people`(`CS_people`) USING BTREE,
  CONSTRAINT `tb_mails_ibfk_1` FOREIGN KEY (`sender`) REFERENCES `tb_users` (`user_id`) ON DELETE RESTRICT ON UPDATE RESTRICT,
  CONSTRAINT `tb_mails_ibfk_2` FOREIGN KEY (`recipient`) REFERENCES `tb_users` (`user_id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Table structure for tb_users
-- ----------------------------
DROP TABLE IF EXISTS `tb_users`;
CREATE TABLE `tb_users`  (
  `user_id` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `passwd` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `question` varchar(40) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `answer` varchar(40) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  PRIMARY KEY (`user_id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Dynamic;

SET FOREIGN_KEY_CHECKS = 1;
