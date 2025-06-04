import 'dart:convert';
import 'package:http/http.dart' as http;
import 'package:flutter_secure_storage/flutter_secure_storage.dart';
import 'dart:io' show Platform;
import 'package:flutter/foundation.dart' show kIsWeb;

class ApiService {
  final _storage = const FlutterSecureStorage();

  static String get _baseUrl {
    if (kIsWeb) {
      return 'http://localhost:3000/api';
    } else if (Platform.isAndroid) {
      return 'http://10.0.2.2:3000/api';
    } else {
      return 'http://localhost:3000/api'; // iOS simulator or desktop
    }
  }

  Future<String?> _getToken() async {
    return await _storage.read(key: 'jwt');
  }

  Map<String, String> _authHeaders(String? token) {
    return {
      'Content-Type': 'application/json',
      if (token != null) 'Authorization': 'Bearer $token',
    };
  }

  Future<List<dynamic>> fetchDoctors({int page = 1, int limit = 20}) async {
    final token = await _getToken();
    final uri = Uri.parse('$_baseUrl/doctors?page=$page&limit=$limit');
    final response = await http.get(uri, headers: _authHeaders(token));
    if (response.statusCode == 200) {
      return jsonDecode(response.body)['data'];
    } else {
      throw Exception('Failed to load doctors');
    }
  }

  Future<void> registerPatient(Map<String, dynamic> body) async {
    final uri = Uri.parse('$_baseUrl/auth/register/patient');
    final response = await http.post(uri,
        headers: {'Content-Type': 'application/json'},
        body: jsonEncode(body));
    if (response.statusCode != 201) {
      throw Exception('Registration failed: ${response.body}');
    }
  }

  Future<void> login(String email, String password, String type) async {
    final uri = Uri.parse('$_baseUrl/auth/login');
    final response = await http.post(uri,
        headers: {'Content-Type': 'application/json'},
        body: jsonEncode({'email': email, 'password': password, 'type': type}));
    if (response.statusCode == 200) {
      final token = jsonDecode(response.body)['token'];
      await _storage.write(key: 'jwt', value: token);
    } else {
      throw Exception('Login failed: ${response.body}');
    }
  }

  Future<void> logout() async {
    final storage = const FlutterSecureStorage();
    await storage.delete(key: 'jwt');
  }


  // Similarly implement POST, PUT/PATCH, DELETE for patients, appointments, etc.
}
