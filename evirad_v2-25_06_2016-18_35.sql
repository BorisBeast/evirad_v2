-- phpMyAdmin SQL Dump
-- version 4.4.13.1deb1
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Jun 25, 2016 at 06:35 PM
-- Server version: 5.6.30-0ubuntu0.15.10.1
-- PHP Version: 5.6.11-1ubuntu3.3

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `evirad_v2`
--
CREATE DATABASE IF NOT EXISTS `evirad_v2` DEFAULT CHARACTER SET utf8 COLLATE utf8_unicode_ci;
USE `evirad_v2`;

-- --------------------------------------------------------

--
-- Table structure for table `citac`
--

CREATE TABLE IF NOT EXISTS `citac` (
  `id` int(10) unsigned NOT NULL,
  `naziv` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `funkcija` enum('u','i','t') COLLATE utf8_unicode_ci NOT NULL,
  `zona` int(10) unsigned NOT NULL,
  `lokacija` int(10) unsigned NOT NULL,
  `registracija_vremena` tinyint(1) DEFAULT '0',
  `kontroler` int(10) unsigned DEFAULT NULL,
  `socket` int(10) unsigned NOT NULL,
  `adresa` varchar(30) CHARACTER SET ascii NOT NULL
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Dumping data for table `citac`
--

INSERT INTO `citac` (`id`, `naziv`, `funkcija`, `zona`, `lokacija`, `registracija_vremena`, `kontroler`, `socket`, `adresa`) VALUES
(1, 'citac1', 'u', 1, 1, 1, NULL, 1, '1'),
(2, 'citac2', 'i', 1, 1, 0, NULL, 2, '2');

-- --------------------------------------------------------

--
-- Table structure for table `firma`
--

CREATE TABLE IF NOT EXISTS `firma` (
  `id` int(10) unsigned NOT NULL,
  `naziv` varchar(50) COLLATE utf8_unicode_ci NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `grupa`
--

CREATE TABLE IF NOT EXISTS `grupa` (
  `id` int(10) unsigned NOT NULL,
  `naziv` varchar(50) COLLATE utf8_unicode_ci NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `interval`
--

CREATE TABLE IF NOT EXISTS `interval` (
  `id` int(10) unsigned NOT NULL,
  `naziv` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `dani_u_sedmici_flags` char(7) CHARACTER SET ascii NOT NULL,
  `datum_pocetak` datetime NOT NULL,
  `datum_kraj` datetime NOT NULL,
  `prioritet` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `is_zabranjeno` tinyint(1) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `kartica`
--

CREATE TABLE IF NOT EXISTS `kartica` (
  `id` int(10) unsigned NOT NULL,
  `kod` varchar(20) CHARACTER SET ascii DEFAULT NULL,
  `radnik` int(10) unsigned DEFAULT NULL,
  `aktivna` tinyint(1) NOT NULL DEFAULT '1',
  `author` int(10) unsigned DEFAULT NULL,
  `editor` int(10) unsigned DEFAULT NULL,
  `edit_time` datetime DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `komentar`
--

CREATE TABLE IF NOT EXISTS `komentar` (
  `id` int(10) unsigned NOT NULL,
  `komentar` varchar(100) COLLATE utf8_unicode_ci NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `kontroler`
--

CREATE TABLE IF NOT EXISTS `kontroler` (
  `id` int(10) unsigned NOT NULL,
  `naziv` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `socket` int(10) unsigned NOT NULL,
  `adresa` varchar(30) CHARACTER SET ascii NOT NULL,
  `lokacija` int(10) unsigned NOT NULL,
  `tip_brave` enum('a','d','m') CHARACTER SET ascii NOT NULL,
  `trajanje_otvaranja` int(10) unsigned NOT NULL,
  `ulazi_maska` varchar(10) COLLATE utf8_unicode_ci NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci COMMENT='kontroler je isto sto i vrata';

-- --------------------------------------------------------

--
-- Table structure for table `korisnik`
--

CREATE TABLE IF NOT EXISTS `korisnik` (
  `id` int(11) unsigned NOT NULL,
  `ime` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `sifra` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `aktivan` tinyint(1) NOT NULL DEFAULT '1',
  `tip` enum('korisnik','admin') CHARACTER SET ascii NOT NULL DEFAULT 'korisnik'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `log_dogadjaja`
--

CREATE TABLE IF NOT EXISTS `log_dogadjaja` (
  `id` bigint(20) unsigned NOT NULL,
  `citac` int(10) unsigned DEFAULT NULL,
  `dogadjaj` enum('u','i') COLLATE utf8_unicode_ci NOT NULL,
  `kod_kartice` varchar(20) COLLATE utf8_unicode_ci DEFAULT NULL,
  `taster` char(1) CHARACTER SET ascii NOT NULL DEFAULT '0' COMMENT 'Taster pritisnut na terminalu (''0'' = Normal)',
  `vrijeme` datetime NOT NULL,
  `lokacija` int(10) unsigned DEFAULT NULL,
  `greska` smallint(5) unsigned NOT NULL DEFAULT '0',
  `kartica` int(10) unsigned DEFAULT NULL,
  `radnik` int(10) unsigned DEFAULT NULL,
  `aktivan` tinyint(1) NOT NULL DEFAULT '1',
  `pravdanje` char(1) COLLATE utf8_unicode_ci NOT NULL DEFAULT '0' COMMENT 'Ima isti opseg vrijednosti kao taster',
  `komentar` int(10) unsigned DEFAULT NULL,
  `author` int(10) unsigned DEFAULT NULL,
  `editor` int(10) unsigned DEFAULT NULL,
  `edit_time` datetime DEFAULT NULL,
  `sync_status` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `sync_id` bigint(20) unsigned DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `lokacija`
--

CREATE TABLE IF NOT EXISTS `lokacija` (
  `id` int(10) unsigned NOT NULL,
  `naziv` varchar(30) COLLATE utf8_unicode_ci NOT NULL
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Dumping data for table `lokacija`
--

INSERT INTO `lokacija` (`id`, `naziv`) VALUES
(1, 'Probna lokacija');

-- --------------------------------------------------------

--
-- Table structure for table `pravo`
--

CREATE TABLE IF NOT EXISTS `pravo` (
  `id` int(10) unsigned NOT NULL,
  `zona` int(10) unsigned NOT NULL,
  `grupa` int(10) unsigned NOT NULL,
  `interval` int(10) unsigned NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `radnik`
--

CREATE TABLE IF NOT EXISTS `radnik` (
  `id` int(10) unsigned NOT NULL,
  `ime` varchar(30) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `prezime` varchar(30) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `komentar` varchar(40) COLLATE utf8_unicode_ci DEFAULT NULL,
  `firma` int(10) unsigned NOT NULL COMMENT 'Sluzi da ogranici da jedna firma mijenja podatke drugoj',
  `sluzba` int(10) unsigned NOT NULL,
  `grupa` int(10) unsigned NOT NULL,
  `aktivan` tinyint(1) NOT NULL DEFAULT '1',
  `author` int(10) unsigned NOT NULL,
  `editor` int(10) unsigned NOT NULL,
  `edit_time` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `sluzba`
--

CREATE TABLE IF NOT EXISTS `sluzba` (
  `id` int(10) unsigned NOT NULL,
  `naziv` varchar(50) COLLATE utf8_unicode_ci NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `socket`
--

CREATE TABLE IF NOT EXISTS `socket` (
  `id` int(10) unsigned NOT NULL,
  `naziv` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `lokacija` int(10) unsigned NOT NULL,
  `tip` char(1) COLLATE utf8_unicode_ci NOT NULL,
  `funkcija` char(1) CHARACTER SET ascii COLLATE ascii_bin DEFAULT NULL,
  `parametri` varchar(100) CHARACTER SET ascii NOT NULL
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Dumping data for table `socket`
--

INSERT INTO `socket` (`id`, `naziv`, `lokacija`, `tip`, `funkcija`, `parametri`) VALUES
(1, 'udp socket', 1, 'u', NULL, '{\r\n  "port": 1337,\r\n  "address": "127.0.0.1"\r\n}'),
(2, 'serial socket', 1, 's', NULL, '{\r\n  "baud": 9600,\r\n  "address": "/dev/ttyUSB0"\r\n}');

-- --------------------------------------------------------

--
-- Table structure for table `zona`
--

CREATE TABLE IF NOT EXISTS `zona` (
  `id` int(10) unsigned NOT NULL,
  `naziv` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `lokacija` int(10) unsigned NOT NULL,
  `firme` varchar(50) COLLATE utf8_unicode_ci NOT NULL COMMENT 'ne znam jos kako ce ovo da funkcionise, treba upisati vise firmi u jedno polje'
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Dumping data for table `zona`
--

INSERT INTO `zona` (`id`, `naziv`, `lokacija`, `firme`) VALUES
(1, 'Default zona', 1, '');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `citac`
--
ALTER TABLE `citac`
  ADD PRIMARY KEY (`id`),
  ADD KEY `lokacija` (`lokacija`),
  ADD KEY `zona` (`zona`),
  ADD KEY `kontroler` (`kontroler`),
  ADD KEY `socket` (`socket`);

--
-- Indexes for table `firma`
--
ALTER TABLE `firma`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `grupa`
--
ALTER TABLE `grupa`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `interval`
--
ALTER TABLE `interval`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `kartica`
--
ALTER TABLE `kartica`
  ADD PRIMARY KEY (`id`),
  ADD KEY `radnik` (`radnik`),
  ADD KEY `author` (`author`),
  ADD KEY `editor` (`editor`);

--
-- Indexes for table `komentar`
--
ALTER TABLE `komentar`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `kontroler`
--
ALTER TABLE `kontroler`
  ADD PRIMARY KEY (`id`),
  ADD KEY `lokacija` (`lokacija`),
  ADD KEY `socket` (`socket`);

--
-- Indexes for table `korisnik`
--
ALTER TABLE `korisnik`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `ime` (`ime`);

--
-- Indexes for table `log_dogadjaja`
--
ALTER TABLE `log_dogadjaja`
  ADD PRIMARY KEY (`id`),
  ADD KEY `author` (`author`),
  ADD KEY `editor` (`editor`),
  ADD KEY `radnik` (`radnik`),
  ADD KEY `kartica` (`kartica`),
  ADD KEY `citac` (`citac`),
  ADD KEY `lokacija` (`lokacija`),
  ADD KEY `komentar` (`komentar`);

--
-- Indexes for table `lokacija`
--
ALTER TABLE `lokacija`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `pravo`
--
ALTER TABLE `pravo`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `zona_2` (`zona`,`grupa`,`interval`),
  ADD KEY `grupa` (`grupa`),
  ADD KEY `interval` (`interval`);

--
-- Indexes for table `radnik`
--
ALTER TABLE `radnik`
  ADD PRIMARY KEY (`id`),
  ADD KEY `sluzba` (`sluzba`),
  ADD KEY `firma` (`firma`),
  ADD KEY `grupa` (`grupa`),
  ADD KEY `author` (`author`),
  ADD KEY `editor` (`editor`);

--
-- Indexes for table `sluzba`
--
ALTER TABLE `sluzba`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `socket`
--
ALTER TABLE `socket`
  ADD PRIMARY KEY (`id`),
  ADD KEY `lokacija` (`lokacija`);

--
-- Indexes for table `zona`
--
ALTER TABLE `zona`
  ADD PRIMARY KEY (`id`),
  ADD KEY `lokacija` (`lokacija`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `citac`
--
ALTER TABLE `citac`
  MODIFY `id` int(10) unsigned NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=3;
--
-- AUTO_INCREMENT for table `firma`
--
ALTER TABLE `firma`
  MODIFY `id` int(10) unsigned NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `grupa`
--
ALTER TABLE `grupa`
  MODIFY `id` int(10) unsigned NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `interval`
--
ALTER TABLE `interval`
  MODIFY `id` int(10) unsigned NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `kartica`
--
ALTER TABLE `kartica`
  MODIFY `id` int(10) unsigned NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `komentar`
--
ALTER TABLE `komentar`
  MODIFY `id` int(10) unsigned NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `kontroler`
--
ALTER TABLE `kontroler`
  MODIFY `id` int(10) unsigned NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `korisnik`
--
ALTER TABLE `korisnik`
  MODIFY `id` int(11) unsigned NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `log_dogadjaja`
--
ALTER TABLE `log_dogadjaja`
  MODIFY `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `lokacija`
--
ALTER TABLE `lokacija`
  MODIFY `id` int(10) unsigned NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=2;
--
-- AUTO_INCREMENT for table `pravo`
--
ALTER TABLE `pravo`
  MODIFY `id` int(10) unsigned NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `radnik`
--
ALTER TABLE `radnik`
  MODIFY `id` int(10) unsigned NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `sluzba`
--
ALTER TABLE `sluzba`
  MODIFY `id` int(10) unsigned NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `socket`
--
ALTER TABLE `socket`
  MODIFY `id` int(10) unsigned NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=3;
--
-- AUTO_INCREMENT for table `zona`
--
ALTER TABLE `zona`
  MODIFY `id` int(10) unsigned NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=2;
--
-- Constraints for dumped tables
--

--
-- Constraints for table `citac`
--
ALTER TABLE `citac`
  ADD CONSTRAINT `citac_ibfk_1` FOREIGN KEY (`lokacija`) REFERENCES `lokacija` (`id`),
  ADD CONSTRAINT `citac_ibfk_3` FOREIGN KEY (`kontroler`) REFERENCES `kontroler` (`id`),
  ADD CONSTRAINT `citac_ibfk_4` FOREIGN KEY (`socket`) REFERENCES `socket` (`id`),
  ADD CONSTRAINT `citac_ibfk_5` FOREIGN KEY (`zona`) REFERENCES `zona` (`id`);

--
-- Constraints for table `kartica`
--
ALTER TABLE `kartica`
  ADD CONSTRAINT `kartica_ibfk_1` FOREIGN KEY (`radnik`) REFERENCES `radnik` (`id`),
  ADD CONSTRAINT `kartica_ibfk_2` FOREIGN KEY (`author`) REFERENCES `korisnik` (`id`),
  ADD CONSTRAINT `kartica_ibfk_3` FOREIGN KEY (`editor`) REFERENCES `korisnik` (`id`);

--
-- Constraints for table `kontroler`
--
ALTER TABLE `kontroler`
  ADD CONSTRAINT `kontroler_ibfk_1` FOREIGN KEY (`lokacija`) REFERENCES `lokacija` (`id`),
  ADD CONSTRAINT `kontroler_ibfk_2` FOREIGN KEY (`socket`) REFERENCES `socket` (`id`);

--
-- Constraints for table `log_dogadjaja`
--
ALTER TABLE `log_dogadjaja`
  ADD CONSTRAINT `log_dogadjaja_ibfk_1` FOREIGN KEY (`author`) REFERENCES `korisnik` (`id`),
  ADD CONSTRAINT `log_dogadjaja_ibfk_2` FOREIGN KEY (`editor`) REFERENCES `korisnik` (`id`),
  ADD CONSTRAINT `log_dogadjaja_ibfk_3` FOREIGN KEY (`kartica`) REFERENCES `kartica` (`id`),
  ADD CONSTRAINT `log_dogadjaja_ibfk_4` FOREIGN KEY (`radnik`) REFERENCES `radnik` (`id`),
  ADD CONSTRAINT `log_dogadjaja_ibfk_6` FOREIGN KEY (`lokacija`) REFERENCES `lokacija` (`id`),
  ADD CONSTRAINT `log_dogadjaja_ibfk_7` FOREIGN KEY (`komentar`) REFERENCES `komentar` (`id`),
  ADD CONSTRAINT `log_dogadjaja_ibfk_8` FOREIGN KEY (`citac`) REFERENCES `citac` (`id`);

--
-- Constraints for table `pravo`
--
ALTER TABLE `pravo`
  ADD CONSTRAINT `pravo_ibfk_2` FOREIGN KEY (`grupa`) REFERENCES `grupa` (`id`),
  ADD CONSTRAINT `pravo_ibfk_3` FOREIGN KEY (`interval`) REFERENCES `interval` (`id`),
  ADD CONSTRAINT `pravo_ibfk_4` FOREIGN KEY (`zona`) REFERENCES `zona` (`id`);

--
-- Constraints for table `radnik`
--
ALTER TABLE `radnik`
  ADD CONSTRAINT `radnik_ibfk_1` FOREIGN KEY (`sluzba`) REFERENCES `sluzba` (`id`),
  ADD CONSTRAINT `radnik_ibfk_2` FOREIGN KEY (`firma`) REFERENCES `firma` (`id`),
  ADD CONSTRAINT `radnik_ibfk_3` FOREIGN KEY (`grupa`) REFERENCES `grupa` (`id`),
  ADD CONSTRAINT `radnik_ibfk_4` FOREIGN KEY (`author`) REFERENCES `korisnik` (`id`),
  ADD CONSTRAINT `radnik_ibfk_5` FOREIGN KEY (`editor`) REFERENCES `korisnik` (`id`);

--
-- Constraints for table `socket`
--
ALTER TABLE `socket`
  ADD CONSTRAINT `socket_ibfk_1` FOREIGN KEY (`lokacija`) REFERENCES `lokacija` (`id`);

--
-- Constraints for table `zona`
--
ALTER TABLE `zona`
  ADD CONSTRAINT `zona_ibfk_1` FOREIGN KEY (`lokacija`) REFERENCES `lokacija` (`id`);

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
