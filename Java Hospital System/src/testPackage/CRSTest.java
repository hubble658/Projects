package src.testPackage;

import org.junit.jupiter.api.Test;
import src.*;
import src.excepitons.DuplicateInfoException;
import src.excepitons.IDException;

import java.time.LocalDate;

import static org.junit.jupiter.api.Assertions.*;

class CRSTest {


    Hospital tmpHospital = new Hospital("Test Hospital", 1453);
    Section tmpSection = new Section("Test Section", 163);
    Section tmpSection2 = new Section("Test Section", 134);
    Doctor tmpDoctor = new Doctor("Test Doctor ", 1231234, 4);
    Patient tmpPatient = new Patient("Test Patient", 12654897);
    Patient tmpPatient2 = new Patient("Test Patient", 38743481);
    LocalDate tmpDate1 = LocalDate.ofYearDay(2025,102);
    LocalDate tmpDate2 = LocalDate.ofYearDay(2025,32);

    Rendezvous tmpRendezvous = new Rendezvous(tmpDate1, tmpPatient, tmpHospital, tmpSection, tmpDoctor);
    Rendezvous tmpRendezvous2 = new Rendezvous(tmpDate2, tmpPatient, tmpHospital, tmpSection, tmpDoctor);
    CRS crs = new CRS();

    @Test
    void hasPatient() {
        crs.addHospital(tmpHospital);
        crs.addPatient(tmpPatient);
        tmpHospital.addSection(tmpSection);
        tmpSection.addDoctor(tmpDoctor);
        tmpDoctor.getSchedule().addRendezvous(tmpRendezvous);

        assertTrue(crs.hasPatient(tmpPatient.getId()));
        assertFalse(crs.hasPatient(tmpPatient2.getId()));
    }

    @Test
    void updateRendezvous() {
        crs.addHospital(tmpHospital);
        tmpHospital.addSection(tmpSection);
        tmpSection.addDoctor(tmpDoctor);

        crs.addPatient(tmpPatient);
        assertTrue(crs.makeRendezvous(tmpPatient.getId(), tmpHospital.getId(), tmpSection.getId(), tmpDoctor.getDiplomaId(), tmpRendezvous.getDateTime()));

        assertEquals(1, crs.getRendezvous().size());

        crs.removeRendezvous(tmpRendezvous);
        crs.updateRendezvous();
        assertEquals(0, crs.getRendezvous().size());

    }

    @Test
    void idExceptionTest() {
        crs.addHospital(tmpHospital);
        tmpHospital.addSection(tmpSection);
        tmpSection.addDoctor(tmpDoctor);

        Throwable exception = assertThrows(IDException.class, () -> crs.makeRendezvous(tmpPatient.getId(), tmpHospital.getId(), tmpSection.getId(), tmpDoctor.getDiplomaId(), tmpRendezvous.getDateTime()));
        assertNotNull(exception.getMessage());

        crs.addPatient(tmpPatient);
        assertTrue(crs.makeRendezvous(tmpPatient.getId(), tmpHospital.getId(), tmpSection.getId(), tmpDoctor.getDiplomaId(), tmpRendezvous.getDateTime()));

        Throwable exception2 = assertThrows(IDException.class, () -> crs.makeRendezvous(tmpPatient.getId(), tmpHospital.getId(), tmpSection2.getId(), tmpDoctor.getDiplomaId(), tmpRendezvous.getDateTime()));
        assertNotNull(exception.getMessage());
    }

}