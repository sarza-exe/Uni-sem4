const bcrypt = require('bcryptjs');
const Patient = require('../models/patient');

// GET /patients?page=&limit=
exports.getAll = async (req, res, next) => {
    try{
        const page = parseInt(req.query.page, 10) || 1;
        const limit = parseInt(req.query.limit, 10) || 20;
        const skip = (page - 1) * limit;

        const [total, patients] = await Promise.all([
            Patient.countDocuments(),
            Patient.find()
                .select('-passwordHash')
                .skip(skip)
                .limit(limit)
                .sort({createdAt: -1})
        ]);

        res.json({
            page,
            limit,
            total,
            data: patients
        });
    } catch (err) {
        next(err);
    }
};


// GET /patients/:id
exports.getById = async (req, res, next) => {
  try {
    const patient = await Patient.findById(req.params.id).select('-passwordHash');
    if (!patient) return res.status(404).json({ error: 'Patient not found' });
    res.json(patient);
  } catch (err) {
    next(err);
  }
};

// PUT /patients/:id
exports.update = async (req, res, next) => {
  try {
    const updates = { ...req.body };

    if (updates.password) {
      updates.passwordHash = await bcrypt.hash(updates.password, 10);
      delete updates.password;
    }

    const patient = await Patient.findByIdAndUpdate(
      req.params.id,
      updates,
      //new: true returns the updated document. runValidators: true ensures validation rules are checked.
      { new: true, runValidators: true }
    ).select('-passwordHash');

    if (!patient) return res.status(404).json({ error: 'Patient not found' });
    res.json(patient);
  } catch (err) {
    next(err);
  }
};

// DELETE /patients/:id
exports.remove = async (req, res, next) => {
  try {
    const patient = await Patient.findByIdAndDelete(req.params.id);
    if (!patient) return res.status(404).json({ error: 'Patient not found' });
    res.status(204).end();
  } catch (err) {
    next(err);
  }
};
