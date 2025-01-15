package src;

import java.util.Scanner;
import userInteraction.ConsoleHospitalSystem;
import userInteraction.Gui;
import java.util.concurrent.CountDownLatch;

import javax.swing.*;

public class Main {
    public static void main(String[] args) {
        try (Scanner scanner = new Scanner(System.in)) {
            CRS crs = new CRS();
            crs.loadTablesFromDisk("data.ser");

            while (true) {
                System.out.println("Which mod do you want to run?");
                System.out.println("1: Console Mod");
                System.out.println("2: Gui Mod");
                System.out.println("0: Exit");
                System.out.print("Make your choice: ");

                String input = scanner.nextLine().trim();

                int menu;
                try {
                    menu = Integer.parseInt(input);
                } catch (NumberFormatException e) {
                    System.out.println("Invalid input! Please enter a number.");
                    continue;
                }

                if (menu == 0) {
                    System.out.println("Exiting...");
                    System.exit(0);
                    break;
                } else if (menu == 1) {
                    try {
                        ConsoleHospitalSystem consoleSystem = new ConsoleHospitalSystem(crs,scanner);
                    } catch (Exception e) {
                        System.out.println("Error in Console Mod: " + e.getMessage());
                    }
                } else if (menu == 2) {
                    try {
                        CountDownLatch latch = new CountDownLatch(1);
                        SwingUtilities.invokeLater(() -> Gui.createAndShowGui(crs, latch));
                        latch.await();
                    } catch (Exception e) {
                        System.out.println("Error in GUI Mod: " + e.getMessage());
                    }
                } else {
                    System.out.println("Invalid choice! Please enter 0, 1, or 2.");
                }
            }
        }
    }
}