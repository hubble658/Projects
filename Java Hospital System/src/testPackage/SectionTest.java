package src.testPackage;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.function.Executable;
import src.*;
import src.excepitons.DuplicateInfoException;
import src.excepitons.IDException;

import java.time.LocalDate;

import static org.junit.jupiter.api.Assertions.*;

class SectionTest {

    Hospital tmpHospital = new Hospital("Test Hospital", 1453);
    Section tmpSection = new Section("Test Section", 163);
    Section tmpSection2 = new Section("Test Section", 134);
    Doctor tmpDoctor = new Doctor("Test Doctor ", 1231234, 4);


    @Test
    void getDoctor() {
        tmpHospital.addSection(tmpSection);
        tmpSection.addDoctor(tmpDoctor);

        assertTrue(tmpSection.getDoctor(tmpDoctor.getDiplomaId()).equals(tmpDoctor));
        assertNull(tmpSection2.getDoctor(tmpDoctor.getDiplomaId()));
    }

    @Test
    void hasDoctor() {
        tmpHospital.addSection(tmpSection);
        tmpSection.addDoctor(tmpDoctor);
        assertTrue(tmpSection.hasDoctor(tmpDoctor.getDiplomaId()));
    }

    @Test
    void addDoctor() {

        tmpHospital.addSection(tmpSection);
        assertDoesNotThrow(() -> tmpSection.addDoctor(tmpDoctor));

        Throwable exception = assertThrows(DuplicateInfoException.class, () -> tmpSection.addDoctor(tmpDoctor));
        assertNotNull(exception.getMessage());
    }

    @Test
    void removeDoctor() {
        tmpHospital.addSection(tmpSection);
        tmpSection.addDoctor(tmpDoctor);
        assertTrue(tmpSection.removeDoctor(tmpDoctor));
        assertFalse(tmpSection.removeDoctor(tmpDoctor));
    }

}