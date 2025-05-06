// src/models/Patient.js
const mongoose = require('mongoose');

const patientSchema = new mongoose.Schema({
  name:         { type: String, required: true },
  birthDate:    { type: Date,   required: true },
  email:        { type: String, required: true, unique: true },
  phone:        { type: String },
  passwordHash: { type: String, required: true }
}, { timestamps: true });

module.exports = mongoose.model('Patient', patientSchema);
