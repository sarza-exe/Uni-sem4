const bcrypt = require('bcryptjs');
const Doctor = require('../models/Doctor');

// GET /doctors?page=&limit=
exports.getAll = async (req, res, next) => {
  try {
    const page = parseInt(req.query.page, 10) || 1;
    const limit = parseInt(req.query.limit, 10) || 20;
    const skip = (page - 1) * limit;

    const [total, doctors] = await Promise.all([
      Doctor.countDocuments(),
      Doctor.find()
        .select('-passwordHash')
        .skip(skip)
        .limit(limit)
        .sort({ createdAt: -1 })
    ]);

    res.json({
      page,
      limit,
      total,
      data: doctors
    });
  } catch (err) {
    next(err);
  }
};

// GET /doctors/:id
exports.getById = async (req, res, next) => {
  try {
    const doctor = await Doctor.findById(req.params.id).select('-passwordHash');
    if (!doctor) return res.status(404).json({ error: 'Doctor not found' });
    res.json(doctor);
  } catch (err) {
    next(err);
  }
};

// POST /doctors
exports.create = async (req, res, next) => {
  try {
    const { name, specialty, email, password, role } = req.body;
    const existing = await Doctor.findOne({ email });
    if (existing) return res.status(400).json({ error: 'Email already in use' });

    const passwordHash = await bcrypt.hash(password, 10);
    const doctor = await Doctor.create({ name, specialty, email, passwordHash, role });
    // exclude passwordHash in response
    const { passwordHash: _, ...doctorData } = doctor.toObject();

    res.status(201).json(doctorData);
  } catch (err) {
    next(err);
  }
};

// PUT /doctors/:id
exports.update = async (req, res, next) => {
  try {
    const updates = { ...req.body };

    if (updates.password) {
      updates.passwordHash = await bcrypt.hash(updates.password, 10);
      delete updates.password;
    }

    const doctor = await Doctor.findByIdAndUpdate(
      req.params.id,
      updates,
      { new: true, runValidators: true }
    ).select('-passwordHash');

    if (!doctor) return res.status(404).json({ error: 'Doctor not found' });
    res.json(doctor);
  } catch (err) {
    next(err);
  }
};

// DELETE /doctors/:id
exports.remove = async (req, res, next) => {
  try {
    const doctor = await Doctor.findByIdAndDelete(req.params.id);
    if (!doctor) return res.status(404).json({ error: 'Doctor not found' });
    res.status(204).end();
  } catch (err) {
    next(err);
  }
};
