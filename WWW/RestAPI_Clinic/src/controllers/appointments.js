// src/controllers/appointments.js
const Appointment = require('../models/Appointment');

// GET /appointments?page=&limit=&doctor=&patient=&date
exports.getAll = async (req, res, next) => {
  try {
    const page = parseInt(req.query.page, 10) || 1;
    const limit = parseInt(req.query.limit, 10) || 20;
    const skip = (page - 1) * limit;

    // Filtering
    const filter = {};
    if (req.query.doctor) filter.doctor = req.query.doctor;
    if (req.query.patient) filter.patient = req.query.patient;
    if (req.query.date) filter.date = { $gte: new Date(req.query.date) };

    const [total, appointments] = await Promise.all([
      Appointment.countDocuments(filter),
      Appointment.find(filter)
        .populate('doctor', 'name specialty')
        .populate('patient', 'name email')
        .sort({ date: 1 })
        .skip(skip)
        .limit(limit)
    ]);

    res.json({ page, limit, total, data: appointments });
  } catch (err) {
    next(err);
  }
};

// GET /appointments/:id
exports.getById = async (req, res, next) => {
  try {
    const appointment = await Appointment.findById(req.params.id)
      .populate('doctor', 'name specialty')
      .populate('patient', 'name email');
    if (!appointment) return res.status(404).json({ error: 'Appointment not found' });
    res.json(appointment);
  } catch (err) {
    next(err);
  }
};

// POST /appointments
exports.create = async (req, res, next) => {
  try {
    const { doctor, patient, date, status, reason } = req.body;
    const appointment = await Appointment.create({ doctor, patient, date, status, reason });
    res.status(201).json(appointment);
  } catch (err) {
    next(err);
  }
};

// PUT /appointments/:id
exports.update = async (req, res, next) => {
  try {
    const updates = { ...req.body };
    const appointment = await Appointment.findByIdAndUpdate(
      req.params.id,
      updates,
      { new: true, runValidators: true }
    );
    if (!appointment) return res.status(404).json({ error: 'Appointment not found' });
    res.json(appointment);
  } catch (err) {
    next(err);
  }
};

// DELETE /appointments/:id
exports.remove = async (req, res, next) => {
  try {
    const appointment = await Appointment.findByIdAndDelete(req.params.id);
    if (!appointment) return res.status(404).json({ error: 'Appointment not found' });
    res.status(204).end();
  } catch (err) {
    next(err);
  }
};