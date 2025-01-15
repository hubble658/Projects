package src.testPackage;

import org.junit.jupiter.api.Test;
import src.*;

import java.time.LocalDate;

import static org.junit.jupiter.api.Assertions.*;

class ScheduleTest {

    Hospital tmpHospital = new Hospital("Test Hospital", 1453);
    Section tmpSection = new Section("Test Section", 163);
    Doctor tmpDoctor = new Doctor("Test Doctor ", 1231234, 4);
    Patient tmpPatient = new Patient("Test Patient", 12654897);
    LocalDate tmpDate1 = LocalDate.ofYearDay(2025,102);
    LocalDate tmpDate2 = LocalDate.ofYearDay(2025,32);

    Rendezvous tmpRendezvous = new Rendezvous(tmpDate1, tmpPatient, tmpHospital, tmpSection, tmpDoctor);
    Rendezvous tmpRendezvous2 = new Rendezvous(tmpDate2, tmpPatient, tmpHospital, tmpSection, tmpDoctor);

    @Test
    void addRendezvous() {
        tmpHospital.addSection(tmpSection);
        tmpSection.addDoctor(tmpDoctor);
        assertTrue(tmpDoctor.getSchedule().addRendezvous(tmpRendezvous));
    }

    @Test
    void removeRendezvous() {
        tmpHospital.addSection(tmpSection);
        tmpSection.addDoctor(tmpDoctor);
        tmpDoctor.getSchedule().addRendezvous(tmpRendezvous);

        assertTrue(tmpDoctor.getSchedule().removeRendezvous(tmpRendezvous));
        assertFalse(tmpDoctor.getSchedule().removeRendezvous(tmpRendezvous2));
    }
}