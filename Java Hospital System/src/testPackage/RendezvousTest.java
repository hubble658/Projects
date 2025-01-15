package src.testPackage;

import org.junit.jupiter.api.Test;
import src.*;

import java.time.LocalDate;

import static org.junit.jupiter.api.Assertions.*;

class RendezvousTest {

    Hospital tmpHospital = new Hospital("Test Hospital", 1453);
    Section tmpSection = new Section("Test Section", 163);
    Doctor tmpDoctor = new Doctor("Test Doctor ", 1231234, 4);
    Patient tmpPatient = new Patient("Test Patient", 12654897);
    LocalDate tmpDate1 = LocalDate.ofYearDay(2025,102);
    LocalDate tmpDate2 = LocalDate.ofYearDay(2025,32);

    Rendezvous tmpRendezvous = new Rendezvous(tmpDate1, tmpPatient, tmpHospital, tmpSection, tmpDoctor);
    Rendezvous tmpRendezvous2 = new Rendezvous(tmpDate2, tmpPatient, tmpHospital, tmpSection, tmpDoctor);

    @Test
    void getPatient() {
        tmpHospital.addSection(tmpSection);
        tmpSection.addDoctor(tmpDoctor);
        tmpDoctor.getSchedule().addRendezvous(tmpRendezvous);
        assertTrue(tmpDoctor.getSchedule().getSessions().getFirst().getPatient().equals(tmpPatient));
    }

    @Test
    void getDateTime() {
        assertTrue(tmpRendezvous.getDateTime().equals(tmpDate1));
    }

    @Test
    void isSameDay() {
        assertFalse(tmpRendezvous.isSameDay(tmpRendezvous2));
        assertTrue(tmpRendezvous.isSameDay(tmpRendezvous));
    }
}