package userInteraction;
import com.jtattoo.plaf.fast.FastLookAndFeel;
import src.*;
import src.excepitons.DuplicateInfoException;

import javax.swing.*;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.text.*;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.time.LocalDate;
import java.time.ZoneId;
import java.util.*;
import java.util.concurrent.CountDownLatch;


public class Gui {

    public JPanel contentPane;
    private JTabbedPane tabbedPane1;
    private JPanel panelRendezvous;
    private JPanel panelHospital;
    private JPanel panelPatients;
    private JList m1_patientList;
    private JTextField m1_patientName;
    private JButton btn_addPatient;
    private JTextField m1_patientId;
    private JButton btn_removeSelectedPatientButton;
    private JPanel panelMain;
    private JLabel m0_infoText;
    private JTree m2_jTree;
    private JTextField m2_hospitalName;
    private JTextField m2_hospitalId;
    private JButton btn_addHospitalButton;
    private JTextField m2_sectionId;
    private JTextField m2_sectionName;
    private JButton btn_addSectionButton;

    private JTree m3_jTree;
    private JTextField m3_hospitalId;
    private JTextField m3_doctorId;
    private JTextField m3_sectionId;
    private JTextField m3_patientId;
    private JSpinner m3_datePicker;
    private JButton btn_makeRendezvous;

    private JButton btn_save;
    private JList m4_rendezvousList;
    private JTextField m4_patientId;
    private JButton btn_checkRendezvous;
    private JTextField m2_doctorName;
    private JTextField m2_doctorId;
    private JTextField m2_doctorDiplomaId;
    private JButton btn_addDoctorButton;
    private JButton btn_removeSelected;
    private JPanel panelCheck;
    private JButton btn_removeRendezvous;

    private ArrayList<Patient> patientList = new ArrayList<>();

    private static CRS crs;
    public Gui(CRS crs) {
        Gui.crs = crs;

        setPanelPatient(crs);
        setPanelHospital(crs);
        setPanelRendezvous(crs);
        setPanelCheck(crs);

        ((AbstractDocument) m1_patientId.getDocument()).setDocumentFilter(new NumericFilter());

        ((AbstractDocument) m2_hospitalId.getDocument()).setDocumentFilter(new NumericFilter());
        ((AbstractDocument) m2_sectionId.getDocument()).setDocumentFilter(new NumericFilter());
        ((AbstractDocument) m2_doctorDiplomaId.getDocument()).setDocumentFilter(new NumericFilter());
        ((AbstractDocument) m2_doctorId.getDocument()).setDocumentFilter(new NumericFilter());

        ((AbstractDocument) m3_hospitalId.getDocument()).setDocumentFilter(new NumericFilter());
        ((AbstractDocument) m3_sectionId.getDocument()).setDocumentFilter(new NumericFilter());
        ((AbstractDocument) m3_doctorId.getDocument()).setDocumentFilter(new NumericFilter());
        ((AbstractDocument) m3_patientId.getDocument()).setDocumentFilter(new NumericFilter());

        ((AbstractDocument) m4_patientId.getDocument()).setDocumentFilter(new NumericFilter());

        btn_save.addActionListener(e -> {
            crs.saveTablesToDisk("data.ser");
            JOptionPane.showMessageDialog(btn_save, "Changes Successfully saved to disk.");
        });

    }


    public static void main(String[] args) {

        crs = new CRS();
        crs.loadTablesFromDisk("data.ser");
        SwingUtilities.invokeLater(() -> {
            JFrame frame = createAndShowGui(crs,null);
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        });

    }

    public static JFrame createAndShowGui(CRS crs, CountDownLatch latch) {

        try {
            UIManager.setLookAndFeel(new FastLookAndFeel());
        } catch (Exception e) {
            e.printStackTrace();
        }

        JFrame frame = new JFrame("Gui");
        Gui gui = new Gui(crs);
        frame.setContentPane(gui.contentPane);
        frame.pack();
        frame.setMinimumSize(new Dimension(1100, 600));
        frame.setVisible(true);

        frame.addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                crs.saveTablesToDisk("data.ser");
                if (latch != null) {
                    latch.countDown(); // Release latch when the window closes
                }
            }
        });

        return frame;
    }

    public void setPanelPatient(CRS crs){

        updatePatientList();
        btn_addPatient.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

                if(m1_patientName.getText().equalsIgnoreCase("") ||
                        m1_patientId.getText().equalsIgnoreCase("")){
                    JOptionPane.showMessageDialog(btn_addPatient,"You have to fill required information.");
                    return;
                }
                String name = m1_patientName.getText();
                String id = m1_patientId.getText();
                if(name != null && id!= null){
                    Patient tmpPatient = new Patient(name,Long.parseLong(id));
                    try {
                        crs.addPatient(tmpPatient);
                        updatePatientList();
                    } catch (DuplicateInfoException ex) {
                        JOptionPane.showMessageDialog(btn_addPatient,"Duplicate Info!!!\nPatient with this id already exists.");
                    }
                }
            }
        });


        btn_removeSelectedPatientButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if(!m1_patientList.isSelectionEmpty()){
                    int selectedIdx = m1_patientList.getSelectedIndex();
                    crs.removePatient(patientList.get(selectedIdx));

                    updatePatientList();
                }else {
                    JOptionPane.showMessageDialog(btn_removeSelectedPatientButton,"You have to select patient to remove.");
                }
            }
        });

    }

    public void updatePatientList(){
        patientList.clear();
        for (Patient p : crs.getPatients().values()) {
            patientList.add(p);  // Add the Patient object to the list
        }

        // Set the JList model with the patientList
        m1_patientList.setListData(patientList.toArray());
    }

    public void setPanelHospital(CRS crs){
        updateHospitalList();

        btn_addHospitalButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if(m2_hospitalName.getText().equalsIgnoreCase("") ||
                        m2_hospitalId.getText().equalsIgnoreCase("")){
                    JOptionPane.showMessageDialog(btn_addHospitalButton,"You have to fill required information.");
                    return;
                }
                String hospitalName = m2_hospitalName.getText();
                String hospitalId = m2_hospitalId.getText();
                if(hospitalName != null && hospitalId != null){
                    Hospital tmpHospital = new Hospital(hospitalName, Integer.parseInt(hospitalId));
                    try {
                        crs.addHospital(tmpHospital);
                        updateHospitalList();
                    } catch (DuplicateInfoException ex) {
                        JOptionPane.showMessageDialog(btn_addHospitalButton,"Duplicate Info!!!\nHospital with this id already exists.");
                    }
                }
            }
        });

        btn_addSectionButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

                String sectionName = m2_sectionName.getText();
                String sectionId = m2_sectionId.getText();

                if(sectionName.equalsIgnoreCase("") ||
                        sectionId.equalsIgnoreCase("")){
                    JOptionPane.showMessageDialog(btn_addSectionButton,"You have to fill required information.");
                    return;
                }

                Section tmpSection = new Section(sectionName, Integer.parseInt(sectionId));
                TreePath path = m2_jTree.getSelectionPath();
                Hospital parentHospital = null;

                if (path == null){
                    JOptionPane.showMessageDialog(btn_addSectionButton,"You must to select hospital to add Section.");
                    return;
                }

                DefaultMutableTreeNode node = (DefaultMutableTreeNode) path.getLastPathComponent();
                Object nodeObject = node.getUserObject();

                if(nodeObject instanceof Hospital){
                    parentHospital = (Hospital) nodeObject;

                } else if(nodeObject instanceof Section){
                    //Add section to same hospital with selected section
                    TreePath parentPath = path.getParentPath();
                    DefaultMutableTreeNode parentNode = (DefaultMutableTreeNode) parentPath.getLastPathComponent();
                    Object parentObject = parentNode.getUserObject();

                    parentHospital = (Hospital) parentObject;
                }
                if(parentHospital == null)
                    return;

                try {
                    parentHospital.addSection(tmpSection);
                    updateHospitalList();
                } catch (DuplicateInfoException ex) {
                    JOptionPane.showMessageDialog(btn_addSectionButton,"Duplicate Info!!!\nSection with this id already exists.");
                }

            }
        });

        btn_addDoctorButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

                String doctorName = m2_doctorName.getText();
                String doctorId = m2_doctorId.getText();
                String diplomaId = m2_doctorDiplomaId.getText();

                //Has information been filled in?
                if(doctorName.equalsIgnoreCase("") ||
                        doctorId.equalsIgnoreCase("")||
                        diplomaId.equalsIgnoreCase("")){
                    JOptionPane.showMessageDialog(btn_addDoctorButton,"You have to fill required information.");
                    return;
                }

                //Has anything selected?
                TreePath path = m2_jTree.getSelectionPath();
                if (path == null){
                    JOptionPane.showMessageDialog(btn_addDoctorButton,"You must to select Section to add Doctor.");
                    return;
                }

                Doctor tmpDoctor = new Doctor(doctorName,Long.parseLong(doctorId) ,Integer.parseInt(diplomaId));
                Section parentSection;
                DefaultMutableTreeNode node = (DefaultMutableTreeNode) path.getLastPathComponent();
                Object nodeObject = node.getUserObject();

                if(nodeObject instanceof Section){
                    parentSection = (Section) nodeObject;
                } else if(nodeObject instanceof Doctor){

                    //Add section to same hospital with selected section
                    TreePath parentPath = path.getParentPath();
                    DefaultMutableTreeNode parentNode = (DefaultMutableTreeNode) parentPath.getLastPathComponent();
                    Object parentObject = parentNode.getUserObject();

                    parentSection = (Section) parentObject;
                } else{
                    JOptionPane.showMessageDialog(btn_addDoctorButton,"You must to select a Section to add Doctor.");
                    return;
                }

                try {
                    parentSection.addDoctor(tmpDoctor);
                    updateHospitalList();
                    updateDoctorList();
                } catch (DuplicateInfoException ex) {
                    JOptionPane.showMessageDialog(btn_addDoctorButton,"Duplicate Info!!!\nSection with this id already exists.");
                }
            }
        });

        btn_removeSelected.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                TreePath path = m2_jTree.getSelectionPath();
                if(path == null){
                    JOptionPane.showMessageDialog(btn_removeSelected, "You must select Doctor, Section or Hospital to remove.");
                    return;
                }

                DefaultMutableTreeNode node = (DefaultMutableTreeNode) path.getLastPathComponent();
                Object nodeObject = node.getUserObject();

                if(nodeObject instanceof Doctor){

                    DefaultMutableTreeNode parentNode = (DefaultMutableTreeNode) node.getParent();
                    Object nodeParentObject = parentNode.getUserObject();

                    //Confirmation
                    int confirmation = 1;
                    confirmation = JOptionPane.showConfirmDialog(btn_removeSelected, "This doctor will be deleted : "+((Doctor) nodeObject).getName()+
                            " \nDo you want to proceed?","Select an option",JOptionPane.YES_NO_OPTION);
                    if(confirmation != 0)
                        return;

                    if ( ((Section)nodeParentObject).removeDoctor((Doctor) nodeObject) )
                        JOptionPane.showMessageDialog(btn_removeSelected, "Doctor successfully removed.");
                    else
                        JOptionPane.showMessageDialog(btn_removeSelected, "Doctor couldn't be removed .");

                }else if(nodeObject instanceof Section){

                    DefaultMutableTreeNode parentNode = (DefaultMutableTreeNode) node.getParent();
                    Object nodeParentObject = parentNode.getUserObject();

                    //Confirmation
                    int confirmation = 1;
                    confirmation = JOptionPane.showConfirmDialog(btn_removeSelected, "This section will be deleted : "+((Section) nodeObject).getName()+
                            " \nDo you want to proceed?","Select an option",JOptionPane.YES_NO_OPTION);
                    if(confirmation != 0)
                        return;

                    if ( ((Hospital)nodeParentObject).removeSection((Section) nodeObject) )
                        JOptionPane.showMessageDialog(btn_removeSelected, "Section successfully removed.");
                    else
                        JOptionPane.showMessageDialog(btn_removeSelected, "Section couldn't be removed .");

                }else if (nodeObject instanceof Hospital){

                    //Confirmation
                    int confirmation = 1;
                    confirmation = JOptionPane.showConfirmDialog(btn_removeSelected, "This hospital will be deleted : "+((Hospital) nodeObject).getName()+
                            " \nDo you want to proceed?","Select an option",JOptionPane.YES_NO_OPTION);
                    if(confirmation != 0)
                        return;

                    if (crs.removeHospital((Hospital) nodeObject))
                        JOptionPane.showMessageDialog(btn_removeSelected, "Hospital successfully removed.");
                    else
                        JOptionPane.showMessageDialog(btn_removeSelected, "Hospital couldn't be removed .");
                }else{
                    JOptionPane.showMessageDialog(btn_removeSelected, "You must select Doctor, Section or Hospital to remove.");
                    return;
                }

                crs.updateRendezvous();
                updateDoctorList();
                updateHospitalList();
            }
        });


        m2_jTree.addTreeSelectionListener(new TreeSelectionListener() {
            @Override
            public void valueChanged(TreeSelectionEvent e) {
                TreePath selectedPath = e.getNewLeadSelectionPath();

                if (selectedPath != null) {
                    // Get the selected node
                    DefaultMutableTreeNode selectedNode = (DefaultMutableTreeNode) selectedPath.getLastPathComponent();
                    Object selectedObject = selectedNode.getUserObject();
                }
            }
        });

    }

    public void updateHospitalList() {

        DefaultTreeModel treeModel = getDefaultTreeModel();
        m2_jTree.setModel(treeModel);
        updateDoctorList();
    }

    public void setPanelRendezvous(CRS crs){
        updateDoctorList();

        SpinnerDateModel dateModel = new SpinnerDateModel(new Date(), null, null, Calendar.DAY_OF_MONTH);
        m3_datePicker.setModel(dateModel);

        // Set date format
        JSpinner.DateEditor dateEditor = new JSpinner.DateEditor(m3_datePicker, "dd-MM-yyyy");
        m3_datePicker.setEditor(dateEditor);


        btn_makeRendezvous.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

                if(m3_patientId.getText().equalsIgnoreCase("") ||
                        m3_hospitalId.getText().equalsIgnoreCase("") ||
                        m3_sectionId.getText().equalsIgnoreCase("")||
                        m3_doctorId.getText().equalsIgnoreCase("")){
                    JOptionPane.showMessageDialog(btn_makeRendezvous,"You have to fill required information.");
                    return;
                }


                if (crs.hasPatient(Long.parseLong(m3_patientId.getText()))){
                    Date selectedDate = (Date) m3_datePicker.getValue();
                    LocalDate desiredDate = selectedDate.toInstant().atZone(ZoneId.systemDefault()).toLocalDate();

                    try {
                        crs.makeRendezvous(Long.parseLong(m3_patientId.getText()),
                                Integer.parseInt(m3_hospitalId.getText()),
                                Integer.parseInt(m3_sectionId.getText()),
                                Integer.parseInt(m3_doctorId.getText()),
                                desiredDate);
                        JOptionPane.showMessageDialog(btn_makeRendezvous,"Rendezvous Added Successfully!");
                    } catch (Exception ex) {
                        JOptionPane.showMessageDialog(btn_makeRendezvous,ex.getMessage());
                    }

                }else{
                    JOptionPane.showMessageDialog(btn_makeRendezvous,"Invalid Patient Id.\nThere is no patient with this Id.");

                }

            }
        });


    }

    public void updateDoctorList(){

        DefaultTreeModel treeModel = getDefaultTreeModel();
        m3_jTree.setModel(treeModel);

        m3_jTree.addTreeSelectionListener(new TreeSelectionListener() {
            @Override
            public void valueChanged(TreeSelectionEvent e) {
                TreePath selectedPath = e.getNewLeadSelectionPath();

                if (selectedPath != null) {
                    // Get the selected node
                    DefaultMutableTreeNode selectedNode = (DefaultMutableTreeNode) selectedPath.getLastPathComponent();
                    Object selectedObject = selectedNode.getUserObject();

                    if (selectedObject instanceof Hospital) {
                        // It's a hospital
                        String h_id = String.valueOf(((Hospital) selectedObject).getId());
                        m3_hospitalId.setText(h_id);
                        m3_sectionId.setText("");
                        m3_doctorId.setText("");

                    } else if (selectedObject instanceof Section) {

                        DefaultMutableTreeNode h_node = (DefaultMutableTreeNode)selectedNode.getParent();

                        String h_id = String.valueOf(((Hospital) h_node.getUserObject()).getId());
                        String s_id = String.valueOf(((Section) selectedObject).getId());
                        m3_hospitalId.setText(h_id);
                        m3_sectionId.setText(s_id);
                        m3_doctorId.setText("");

                    }else if (selectedObject instanceof Doctor) {
                        DefaultMutableTreeNode h_node = (DefaultMutableTreeNode)selectedNode.getParent().getParent();
                        DefaultMutableTreeNode s_node = (DefaultMutableTreeNode)selectedNode.getParent();

                        String h_id = String.valueOf(((Hospital) h_node.getUserObject()).getId());
                        String s_id = String.valueOf(((Section) s_node.getUserObject()).getId());
                        String d_id = String.valueOf(((Doctor) selectedObject).getDiplomaId());
                        m3_hospitalId.setText(h_id);
                        m3_sectionId.setText(s_id);
                        m3_doctorId.setText(d_id);

                    }
                }
            }
        });
    }

    private static DefaultTreeModel getDefaultTreeModel() {
        DefaultMutableTreeNode root = new DefaultMutableTreeNode("Hospitals");

        // For each hospital, create a node and add it to the root
        for (Hospital hospital : crs.getHospitals().values()) {
            DefaultMutableTreeNode hospitalNode = new DefaultMutableTreeNode(hospital);  // Store the Hospital object

            // For each section in the hospital, create a section node
            for (Section section : hospital.getSections()) {
                DefaultMutableTreeNode sectionNode = new DefaultMutableTreeNode(section);  // Store the Section object
                hospitalNode.add(sectionNode);  // Add section to hospital node
                for (Doctor doctor : section.getDoctors()) {
                    DefaultMutableTreeNode doctorNode = new DefaultMutableTreeNode(doctor);  // Store the Section object
                    sectionNode.add(doctorNode);  // Add section to hospital node
                }
            }
            root.add(hospitalNode);  // Add hospital to the root
        }

        // Create a tree model and set it on the JTree
        return new DefaultTreeModel(root);
    }


    private void setPanelCheck(CRS crs) {

        btn_checkRendezvous.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

                if(m4_patientId.getText().equalsIgnoreCase("")){
                    JOptionPane.showMessageDialog(btn_checkRendezvous,"You have to fill required information.");
                    return;
                }
                String id = m4_patientId.getText();
                if(crs.hasPatient(Long.parseLong(id)) ){
                    updateRendezvousList(Long.parseLong(id));
                }else{
                    JOptionPane.showMessageDialog(btn_checkRendezvous,"There is no patient with this id.");
                    m4_rendezvousList.setListData(new Vector());
                }
            }
        });

        btn_removeRendezvous.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                Object nodeObject = m4_rendezvousList.getSelectedValue();
                Rendezvous tmpRendezvous = null;

                if(nodeObject==null){
                     JOptionPane.showMessageDialog(btn_removeRendezvous,"You have to select rendezvous to remove.");
                    return;
                }

                //Confirmation
                int confirmation;
                confirmation = JOptionPane.showConfirmDialog(btn_removeRendezvous, "This rendezvous will be deleted "+
                        "\nDo you want to proceed?","Select an option",JOptionPane.YES_NO_OPTION);
                if(confirmation != 0)
                    return;

                tmpRendezvous = (Rendezvous) nodeObject;
                if(crs.removeRendezvous(tmpRendezvous))
                    JOptionPane.showMessageDialog(btn_removeRendezvous,"Rendezvous Successfully removed.");
                else
                    JOptionPane.showMessageDialog(btn_removeRendezvous,"Rendezvous couldn't be removed.");

                updateRendezvousList(tmpRendezvous.getPatient().getId());
            }
        });
    }

    private void updateRendezvousList(long patient_Id){
        crs.updateRendezvous();
        ArrayList<Rendezvous> tmpList = new ArrayList<>();
        for (Rendezvous r : crs.getRendezvous())
            if(r.getPatient().getId() == patient_Id)
                tmpList.add(r);

        tmpList.sort(Comparator.comparing(Rendezvous::getDateTime));
        m4_rendezvousList.setListData(tmpList.toArray());

    }

    class NumericFilter extends DocumentFilter {
        @Override
        public void insertString(FilterBypass fb, int offset, String string, AttributeSet attr) throws BadLocationException {
            if (isNumeric(string)) {
                super.insertString(fb, offset, string, attr);
            }
        }

        @Override
        public void replace(FilterBypass fb, int offset, int length, String text, AttributeSet attrs) throws BadLocationException {
            if (isNumeric(text)) {
                super.replace(fb, offset, length, text, attrs);
            }
        }

        @Override
        public void remove(FilterBypass fb, int offset, int length) throws BadLocationException {
            super.remove(fb, offset, length);
        }

        private boolean isNumeric(String text) {
            return text.matches("\\d*"); // Only allows digits (0-9)
        }
    }

}
