// src/controllers/auth.js
const jwt = require('jsonwebtoken');
const bcrypt = require('bcryptjs');
const Doctor = require('../models/Doctor');
const Patient = require('../models/patient');

// POST /auth/register/patient
exports.registerPatient = async (req, res, next) => {
  try {
    const { name, birthDate, email, phone, password } = req.body;
    const existing = await Patient.findOne({ email });
    if (existing) return res.status(409).json({ error: 'Email in use' });

    const passwordHash = await bcrypt.hash(password, 10);
    const patient = await Patient.create({ name, birthDate, email, phone, passwordHash });

    const token = jwt.sign(
      { id: patient._id, role: patient.role },
      process.env.JWT_SECRET,
      { expiresIn: '14d' }
    );

    res.status(201).json({ token });
  } catch (err) {
    next(err);
  }
};

// POST /auth/register/doctor  (admin only)
exports.registerDoctor = async (req, res, next) => {
  try {
    const { name, specialty, email, password, role } = req.body;
    const existing = await Doctor.findOne({ email });
    if (existing) return res.status(409).json({ error: 'Email in use' });

    const passwordHash = await bcrypt.hash(password, 10);
    const doctor = await Doctor.create({ name, specialty, email, passwordHash, role });

    const token = jwt.sign(
      { id: doctor._id, role: doctor.role },
      process.env.JWT_SECRET,
      { expiresIn: '14d' }
    );

    res.status(201).json({ token });
  } catch (err) {
    next(err);
  }
};

// POST /auth/login
exports.login = async (req, res, next) => {
  try {
    const { email, password, type } = req.body;
    const Model = type === 'doctor' ? Doctor : Patient;
    const user = await Model.findOne({ email });
    if (!user) return res.status(400).json({ error: 'Invalid credentials' });

    const isMatch = await bcrypt.compare(password, user.passwordHash);
    if (!isMatch) return res.status(400).json({ error: 'Invalid credentials' });

    const token = jwt.sign(
      { id: user._id, role: user.role },
      process.env.JWT_SECRET,
      { expiresIn: '1h' }
    );
    res.json({ token });
  } catch (err) {
    next(err);
  }
};