#!/usr/bin/env python3
"""
Joystick Converter Configuration Tool

GUI application for configuring the joystick converter device.
"""

import sys
import serial
import serial.tools.list_ports
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QPushButton, QComboBox, QLabel, QTableWidget, QTableWidgetItem,
    QMessageBox, QGroupBox, QDialog, QDialogButtonBox, QFormLayout,
    QSpinBox, QTextEdit
)
from PyQt6.QtCore import Qt, QTimer


class MacroEditorDialog(QDialog):
    """Dialog for editing macros"""
    
    def __init__(self, parent=None, macro_data=None):
        super().__init__(parent)
        self.setWindowTitle("Macro Editor")
        self.setMinimumWidth(600)
        
        layout = QVBoxLayout()
        
        # Macro ID
        form_layout = QFormLayout()
        self.macro_id_spin = QSpinBox()
        self.macro_id_spin.setRange(0, 15)
        if macro_data:
            self.macro_id_spin.setValue(macro_data.get('id', 0))
        form_layout.addRow("Macro ID:", self.macro_id_spin)
        layout.addLayout(form_layout)
        
        # Macro steps
        layout.addWidget(QLabel("Macro Steps (one per line):"))
        self.steps_text = QTextEdit()
        self.steps_text.setPlaceholderText(
            "Example format:\n"
            "key_press 0x04 # A key\n"
            "delay 100\n"
            "key_release\n"
            "mouse_move 10 10\n"
            "mouse_button_press 1\n"
            "delay 50\n"
            "mouse_button_release"
        )
        if macro_data and 'steps' in macro_data:
            self.steps_text.setPlainText(macro_data['steps'])
        layout.addWidget(self.steps_text)
        
        # Buttons
        button_box = QDialogButtonBox(
            QDialogButtonBox.StandardButton.Ok | 
            QDialogButtonBox.StandardButton.Cancel
        )
        button_box.accepted.connect(self.accept)
        button_box.rejected.connect(self.reject)
        layout.addWidget(button_box)
        
        self.setLayout(layout)
    
    def get_macro_data(self):
        """Get macro data from dialog"""
        return {
            'id': self.macro_id_spin.value(),
            'steps': self.steps_text.toPlainText()
        }


class ConfigTool(QMainWindow):
    """Main configuration window"""
    
    def __init__(self):
        super().__init__()
        self.serial_port = None
        self.button_mappings = []
        self.macros = []
        
        self.init_ui()
        self.refresh_ports()
        
        # Auto-refresh port list
        self.port_refresh_timer = QTimer()
        self.port_refresh_timer.timeout.connect(self.refresh_ports)
        self.port_refresh_timer.start(2000)  # Refresh every 2 seconds
    
    def init_ui(self):
        """Initialize user interface"""
        self.setWindowTitle("Joystick Converter Configuration Tool")
        self.setMinimumSize(800, 600)
        
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        
        # Connection section
        connection_group = QGroupBox("Connection")
        connection_layout = QHBoxLayout()
        
        connection_layout.addWidget(QLabel("Port:"))
        self.port_combo = QComboBox()
        connection_layout.addWidget(self.port_combo)
        
        self.connect_button = QPushButton("Connect")
        self.connect_button.clicked.connect(self.toggle_connection)
        connection_layout.addWidget(self.connect_button)
        
        self.refresh_button = QPushButton("Refresh")
        self.refresh_button.clicked.connect(self.refresh_ports)
        connection_layout.addWidget(self.refresh_button)
        
        connection_layout.addStretch()
        connection_group.setLayout(connection_layout)
        main_layout.addWidget(connection_group)
        
        # Output type section
        output_group = QGroupBox("Output Type")
        output_layout = QHBoxLayout()
        
        output_layout.addWidget(QLabel("Output Device:"))
        self.output_combo = QComboBox()
        self.output_combo.addItems(["Gamepad", "Keyboard", "Mouse", "Keyboard+Mouse"])
        self.output_combo.currentIndexChanged.connect(self.on_output_changed)
        output_layout.addWidget(self.output_combo)
        
        output_layout.addStretch()
        output_group.setLayout(output_layout)
        main_layout.addWidget(output_group)
        
        # Button mapping section
        mapping_group = QGroupBox("Button Mappings")
        mapping_layout = QVBoxLayout()
        
        self.mapping_table = QTableWidget()
        self.mapping_table.setColumnCount(4)
        self.mapping_table.setHorizontalHeaderLabels([
            "Source Button", "Mapping Type", "Target", "Action"
        ])
        mapping_layout.addWidget(self.mapping_table)
        
        mapping_buttons = QHBoxLayout()
        add_mapping_btn = QPushButton("Add Mapping")
        add_mapping_btn.clicked.connect(self.add_mapping)
        mapping_buttons.addWidget(add_mapping_btn)
        
        remove_mapping_btn = QPushButton("Remove Mapping")
        remove_mapping_btn.clicked.connect(self.remove_mapping)
        mapping_buttons.addWidget(remove_mapping_btn)
        
        mapping_buttons.addStretch()
        mapping_layout.addLayout(mapping_buttons)
        
        mapping_group.setLayout(mapping_layout)
        main_layout.addWidget(mapping_group)
        
        # Macro section
        macro_group = QGroupBox("Macros")
        macro_layout = QVBoxLayout()
        
        self.macro_table = QTableWidget()
        self.macro_table.setColumnCount(3)
        self.macro_table.setHorizontalHeaderLabels([
            "Macro ID", "Steps", "Action"
        ])
        macro_layout.addWidget(self.macro_table)
        
        macro_buttons = QHBoxLayout()
        add_macro_btn = QPushButton("Add Macro")
        add_macro_btn.clicked.connect(self.add_macro)
        macro_buttons.addWidget(add_macro_btn)
        
        edit_macro_btn = QPushButton("Edit Macro")
        edit_macro_btn.clicked.connect(self.edit_macro)
        macro_buttons.addWidget(edit_macro_btn)
        
        remove_macro_btn = QPushButton("Remove Macro")
        remove_macro_btn.clicked.connect(self.remove_macro)
        macro_buttons.addWidget(remove_macro_btn)
        
        macro_buttons.addStretch()
        macro_layout.addLayout(macro_buttons)
        
        macro_group.setLayout(macro_layout)
        main_layout.addWidget(macro_group)
        
        # Control buttons
        control_layout = QHBoxLayout()
        
        load_btn = QPushButton("Load from Device")
        load_btn.clicked.connect(self.load_config)
        control_layout.addWidget(load_btn)
        
        save_btn = QPushButton("Save to Device")
        save_btn.clicked.connect(self.save_config)
        control_layout.addWidget(save_btn)
        
        control_layout.addStretch()
        main_layout.addLayout(control_layout)
    
    def refresh_ports(self):
        """Refresh available serial ports"""
        current_port = self.port_combo.currentText()
        self.port_combo.clear()
        
        ports = serial.tools.list_ports.comports()
        for port in ports:
            self.port_combo.addItem(f"{port.device} - {port.description}")
        
        # Try to restore previous selection
        index = self.port_combo.findText(current_port)
        if index >= 0:
            self.port_combo.setCurrentIndex(index)
    
    def toggle_connection(self):
        """Connect or disconnect from device"""
        if self.serial_port and self.serial_port.is_open:
            self.disconnect_device()
        else:
            self.connect_device()
    
    def connect_device(self):
        """Connect to device"""
        port_text = self.port_combo.currentText()
        if not port_text:
            QMessageBox.warning(self, "Error", "No port selected")
            return
        
        port = port_text.split(" - ")[0]
        
        try:
            self.serial_port = serial.Serial(port, 115200, timeout=1)
            self.connect_button.setText("Disconnect")
            self.statusBar().showMessage(f"Connected to {port}")
        except Exception as e:
            QMessageBox.critical(self, "Connection Error", str(e))
    
    def disconnect_device(self):
        """Disconnect from device"""
        if self.serial_port:
            self.serial_port.close()
            self.serial_port = None
        self.connect_button.setText("Connect")
        self.statusBar().showMessage("Disconnected")
    
    def on_output_changed(self, index):
        """Handle output type change"""
        output_type = ["Gamepad", "Keyboard", "Mouse", "Combo"][index]
        self.statusBar().showMessage(f"Output type changed to {output_type}")
    
    def add_mapping(self):
        """Add button mapping"""
        # TODO: Show dialog to add mapping
        QMessageBox.information(self, "Add Mapping", "Mapping dialog not yet implemented")
    
    def remove_mapping(self):
        """Remove selected button mapping"""
        current_row = self.mapping_table.currentRow()
        if current_row >= 0:
            self.mapping_table.removeRow(current_row)
    
    def add_macro(self):
        """Add new macro"""
        dialog = MacroEditorDialog(self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            macro_data = dialog.get_macro_data()
            self.macros.append(macro_data)
            self.update_macro_table()
    
    def edit_macro(self):
        """Edit selected macro"""
        current_row = self.macro_table.currentRow()
        if current_row >= 0 and current_row < len(self.macros):
            macro_data = self.macros[current_row]
            dialog = MacroEditorDialog(self, macro_data)
            if dialog.exec() == QDialog.DialogCode.Accepted:
                self.macros[current_row] = dialog.get_macro_data()
                self.update_macro_table()
    
    def remove_macro(self):
        """Remove selected macro"""
        current_row = self.macro_table.currentRow()
        if current_row >= 0 and current_row < len(self.macros):
            self.macros.pop(current_row)
            self.update_macro_table()
    
    def update_macro_table(self):
        """Update macro table display"""
        self.macro_table.setRowCount(len(self.macros))
        for i, macro in enumerate(self.macros):
            self.macro_table.setItem(i, 0, QTableWidgetItem(str(macro['id'])))
            step_count = len(macro['steps'].split('\n')) if macro['steps'] else 0
            self.macro_table.setItem(i, 1, QTableWidgetItem(f"{step_count} steps"))
    
    def load_config(self):
        """Load configuration from device"""
        if not self.serial_port or not self.serial_port.is_open:
            QMessageBox.warning(self, "Error", "Not connected to device")
            return
        
        # TODO: Implement config loading protocol
        QMessageBox.information(self, "Load Config", "Config loading not yet implemented")
    
    def save_config(self):
        """Save configuration to device"""
        if not self.serial_port or not self.serial_port.is_open:
            QMessageBox.warning(self, "Error", "Not connected to device")
            return
        
        # TODO: Implement config saving protocol
        QMessageBox.information(self, "Save Config", "Config saving not yet implemented")
    
    def closeEvent(self, event):
        """Handle window close"""
        self.disconnect_device()
        event.accept()


def main():
    """Main entry point"""
    app = QApplication(sys.argv)
    app.setStyle('Fusion')  # Modern look
    
    window = ConfigTool()
    window.show()
    
    sys.exit(app.exec())


if __name__ == '__main__':
    main()
