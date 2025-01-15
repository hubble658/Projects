package src.testPackage;

import org.junit.jupiter.api.Test;
import src.Doctor;

import static org.junit.jupiter.api.Assertions.*;

class DoctorTest {

    Doctor tmpDoctor = new Doctor("TestName", 121563,12,30);

    @Test
    void getSchedule() {
        assertTrue(tmpDoctor.getSchedule().getSessions().size() < 30);
    }
}