package src.testPackage;

import org.junit.jupiter.api.Test;
import src.Doctor;
import src.Hospital;
import src.Section;

import static org.junit.jupiter.api.Assertions.*;

class HospitalTest {

    Hospital tmpHospital = new Hospital("Test Hospital", 1453);
    Section tmpSection = new Section("Test Section", 163);
    Doctor tmpDoctor = new Doctor("Test Doctor ", 1231234, 4);

    @Test
    void addSection() {
        tmpHospital.addSection(tmpSection);
        assertTrue(tmpHospital.hasSection(tmpSection.getId()));
    }

    @Test
    void removeSection() {
        tmpHospital.addSection(tmpSection);
        tmpHospital.removeSection(tmpSection);
        assertFalse(tmpHospital.hasSection(tmpSection.getId()));
    }

    @Test
    void hasDoctor() {
        tmpHospital.addSection(tmpSection);
        assertFalse(tmpHospital.hasDoctor(tmpDoctor.getDiplomaId()));
        tmpSection.addDoctor(tmpDoctor);
        assertTrue(tmpHospital.hasDoctor(tmpDoctor.getDiplomaId()));
    }

}