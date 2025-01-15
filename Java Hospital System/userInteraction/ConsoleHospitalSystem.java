package userInteraction;

import src.*;
import src.excepitons.IDException;

import java.time.LocalDate;
import java.time.format.DateTimeFormatter;
import java.util.Scanner;

public class ConsoleHospitalSystem {
    private static CRS crs;
    private static Scanner scanner;
    private static DateTimeFormatter dtf = DateTimeFormatter.ofPattern("dd-MM-yyyy");

    public ConsoleHospitalSystem(CRS crs, Scanner scanner) {
        this.crs = crs;
        this.scanner = scanner;


        // Ana döngü
        while (true) {
            System.out.println("\n=== Hospital Management System ===");
            System.out.println("1. Patient Operations");
            System.out.println("2. Hospital Operations");
            System.out.println("3. Add Operations");
            System.out.println("4. Check Rendezvous");
            System.out.println("5. Save Data");
            System.out.println("6. Load Data");
            System.out.println("0. Exit");

            System.out.print("Select operation: ");
            int choice;
            try {
                choice = scanner.nextInt();
                scanner.nextLine();
            } catch (RuntimeException e) {
                choice = 9; // Geçersiz giriş
//                scanner.nextLine(); // Hatalı girdiyi temizle
            }

            switch (choice) {
                case 1:
                    patientOperations();
                    break;
                case 2:
                    hospitalOperations();
                    break;
                case 3:
                    rendezvousOperations();
                    break;
                case 4:
                    checkRendezvous();
                    break;
                case 5:
                    crs.saveTablesToDisk("data.ser");
                    System.out.println("Data saved successfully!");
                    break;
                case 6:
                    crs.loadTablesFromDisk("data.ser");
                    System.out.println("Data loaded successfully!");
                    break;
                case 0:
                    System.out.println("Exiting...");
                    return; // Çıkış
                default:
                    System.out.println("Invalid choice!");
            }
        }
    }

    public static void patientOperations() {
        while(true) {
            System.out.println("\n=== Patient Operations ===");
            System.out.println("1. Add Patient");
            System.out.println("2. Remove Patient");
            System.out.println("3. List Patients");
            System.out.println("0. Back to Main Menu");

            System.out.print("Select operation: ");
            int choice = scanner.nextInt();
            scanner.nextLine(); // Clear buffer

            switch(choice) {
                case 1:
                    System.out.print("Enter patient name: ");
                    String name = scanner.nextLine();
                    System.out.print("Enter patient ID: ");
                    long id = scanner.nextLong();
                    try {
                        crs.addPatient(new Patient(name, id));
                        System.out.println("Patient added successfully!");
                    } catch (Exception e) {
                        System.out.println("Error: " + e.getMessage());
                    }
                    break;
                case 2:
                    System.out.print("Enter patient ID to remove: ");
                    long removeId = scanner.nextLong();
                    Patient patientToRemove = crs.getPatients().get(removeId);
                    if(patientToRemove != null) {
                        if (crs.removePatient(patientToRemove))
                            System.out.println("Patient removed successfully!");
                        else
                            System.out.println("Patient couldn't be removed!");
                    } else {
                        System.out.println("Patient not found!");
                    }
                    break;
                case 3:
                    System.out.println("\nPatient List:");
                    for(Patient p : crs.getPatients().values()) {
                        System.out.println("ID: " + p.getId() + ", Name: " + p.getName());
                    }
                    break;
                case 0:
                    return;
                default:
                    System.out.println("Invalid choice!");
            }
        }
    }

    public static void hospitalOperations() {
        while(true) {
            System.out.println("\n=== Hospital Operations ===");
            System.out.println("1. Add Hospital");
            System.out.println("2. Add Section to Hospital");
            System.out.println("3. Add Doctor to Section");
            System.out.println("4. List Hospitals");
            System.out.println("0. Back to Main Menu");

            System.out.print("Select operation: ");
            int choice = scanner.nextInt();
            scanner.nextLine(); // Clear buffer

            switch(choice) {
                case 1:
                    System.out.print("Enter hospital name: ");
                    String name = scanner.nextLine();
                    System.out.print("Enter hospital ID: ");
                    int id = scanner.nextInt();
                    scanner.nextLine(); // Clear buffer
                    try {
                        crs.addHospital(new Hospital(name, id));
                        System.out.println("Hospital added successfully!");
                    } catch (Exception e) {
                        System.out.println("Error: " + e.getMessage());
                    }
                    break;
                case 2:
                    addSectionToHospital();
                    break;
                case 3:
                    addDoctorToSection();
                    break;
                case 4:
                    listHospitalStructure();
                    break;
                case 0:
                    return;
                default:
                    System.out.println("Invalid choice!");
            }
        }
    }

    private static void addSectionToHospital() {
        System.out.print("Enter hospital ID: ");
        int hospitalId = scanner.nextInt();
        scanner.nextLine();
        Hospital hospital = crs.getHospitals().get(hospitalId);

        if(hospital != null) {
            System.out.print("Enter section name: ");
            String sectionName = scanner.nextLine();
            System.out.print("Enter section ID: ");
            int sectionId = scanner.nextInt();
            scanner.nextLine();
            try {
                hospital.addSection(new Section(sectionName, sectionId));
                System.out.println("Section added successfully!");
            } catch (Exception e) {
                System.out.println("Error: " + e.getMessage());
            }
        } else {
            System.out.println("Hospital not found!");
        }
    }

    private static void addDoctorToSection() {
        System.out.print("Enter hospital ID: ");
        int hospitalId = scanner.nextInt();
        scanner.nextLine();
        Hospital hospital = crs.getHospitals().get(hospitalId);

        if(hospital != null) {
            System.out.print("Enter section ID: ");
            int sectionId = scanner.nextInt();
            scanner.nextLine();
            Section section = null;

            for(Section s : hospital.getSections()) {
                if(s.getId() == sectionId) {
                    section = s;
                    break;
                }
            }

            if(section != null) {
                System.out.print("Enter doctor name: ");
                String doctorName = scanner.nextLine();
                System.out.print("Enter doctor ID: ");
                long doctorId = scanner.nextLong();
                System.out.print("Enter diploma ID: ");
                int diplomaId = scanner.nextInt();
                scanner.nextLine();

                try {
                    section.addDoctor(new Doctor(doctorName, doctorId, diplomaId));
                    System.out.println("Doctor added successfully!");
                } catch (Exception e) {
                    System.out.println("Error: " + e.getMessage());
                }
            } else {
                System.out.println("Section not found!");
            }
        } else {
            System.out.println("Hospital not found!");
        }
    }

    public static void rendezvousOperations() {
        System.out.print("Enter patient ID: ");
        long patientId = scanner.nextLong();
        System.out.print("Enter hospital ID: ");
        int hospitalId = scanner.nextInt();
        System.out.print("Enter section ID: ");
        int sectionId = scanner.nextInt();
        System.out.print("Enter doctor ID: ");
        int doctorId = scanner.nextInt();

        System.out.println("Enter date (dd-MM-yyyy): ");
        scanner.nextLine(); // Clear buffer
        String dateStr = scanner.nextLine();

        LocalDate date = null;
        try {
             date = LocalDate.parse(dateStr,dtf);
        } catch (RuntimeException e) {
            System.out.println("Wrong date format.");
        }

        try {
            crs.makeRendezvous(patientId, hospitalId, sectionId, doctorId, date);
            System.out.println("Rendezvous created successfully!");
        } catch (IDException e) {
            System.out.println(e.getMessage());
        }
    }

    public static void checkRendezvous() {
        System.out.print("Enter patient ID: ");
        long patientId = scanner.nextLong();
        scanner.nextLine();

        if(!crs.hasPatient(patientId)){
            System.out.println("There is no patient with this ID: "+patientId);
            return;
        }

        System.out.println("\nRendezvous List for Patient ID: " + patientId);
        for(Rendezvous r : crs.getRendezvous()) {
            if(r.getPatient().getId() == patientId) {
                System.out.println("Date: " + dtf.format(r.getDateTime()) +
                        ", Hospital: " + r.getHospital().getName() +
                        ", Section: " + r.getSection().getName() +
                        ", Doctor: " + r.getDoctor().getName());
            }
        }
    }

    private static void listHospitalStructure() {
        System.out.println("\nHospital Structure:");
        for(Hospital h : crs.getHospitals().values()) {
            System.out.println("\nHospital: " + h.getName() + " (ID: " + h.getId() + ")");
            for(Section s : h.getSections()) {
                System.out.println("  Section: " + s.getName() + " (ID: " + s.getId() + ")");
                for(Doctor d : s.getDoctors()) {
                    System.out.println("    Doctor: " + d.getName() +
                            " (ID: " + d.getId() +
                            ", Diploma ID: " + d.getDiplomaId() + ")");
                }
            }
        }
    }
}