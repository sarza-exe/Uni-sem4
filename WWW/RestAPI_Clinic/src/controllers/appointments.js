// src/controllers/appointments.js
const Appointment = require('../models/appointment');

// GET /appointments?page=&limit=&doctor=&patient=&date
exports.getAll = async (req, res, next) => {
  try {
    const page  = parseInt(req.query.page, 10)  || 1;
    const limit = parseInt(req.query.limit, 10) || 20;
    const skip  = (page - 1) * limit;

    // Build filter
    const filter = {};
    if (req.query.doctor)  filter.doctor  = req.query.doctor;
    if (req.query.patient) filter.patient = req.query.patient;
    if (req.query.date)    filter.date    = { $gte: new Date(req.query.date) };

    // Authorization: if filtering by patient, only that patient or doctor/admin
    if (filter.patient) {
      const pid = req.user.id;
      const role = req.user.role;
      if (role !== 'admin' && role !== 'doctor' && pid !== filter.patient) {
        return res.status(403).json({ error: 'Forbidden' });
      }
    }

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
    const appt = await Appointment.findById(req.params.id)
      .populate('doctor', 'name specialty email')
      .populate('patient', 'name email');
    if (!appt) return res.status(404).json({ error: 'Appointment not found' });

    const uid = req.user.id;
    const role = req.user.role;
    if (role !== 'admin' && role !== 'doctor' && uid !== appt.patient.id) {
      return res.status(403).json({ error: 'Forbidden' });
    }
    res.json(appt);
  } catch (err) {
    next(err);
  }
};

// POST /appointments
exports.create = async (req, res, next) => {
  try {
    const { doctor, patient, date, reason } = req.body;
    const uid = req.user.id;
    const role = req.user.role;
    if (role !== 'admin' && role !== 'doctor' && uid !== patient) {
      return res.status(403).json({ error: 'Forbidden' });
    }
    const status = (role === 'patient') ? 'awaiting approval' : 'scheduled';
    const appt = await Appointment.create({ doctor, patient, date, status, reason });
    res.status(201).json(appt);
  } catch (err) {
    next(err);
  }
};

// PUT /appointments/:id
exports.update = async (req, res, next) => {
  try {
    const uid = req.user.id;
    const targetId = req.params.id;
    // Only the referring doctor can update
    if (uid !== targetId) {
      return res.status(403).json({ error: 'Forbidden' });
    }

    const appt = await Appointment.findById(req.params.id);
    if (!appt) return res.status(404).json({ error: 'Appointment not found' });
    const updates = { ...req.body };
    const updatedAppt = await Appointment.findByIdAndUpdate(
      req.params.id,
      updates,
      { new: true, runValidators: true }
    );
    res.json(updatedAppt);
  } catch (err) {
    next(err);
  }
};

// DELETE /appointments/:id
exports.remove = async (req, res, next) => {
  try {
    const appt = await Appointment.findById(req.params.id);
    if (!appt) return res.status(404).json({ error: 'Appointment not found' });

    const uid = req.user.id;
    const role = req.user.role;
    // allow delete by doctor or the patient
    if (role !== 'admin' && uid !== appt.doctor.toString() && uid !== appt.patient.toString()) {
      return res.status(403).json({ error: 'Forbidden' });
    }

    await Appointment.findByIdAndDelete(req.params.id);
    res.status(204).end();
  } catch (err) {
    next(err);
  }
};