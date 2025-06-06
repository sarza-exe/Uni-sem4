import 'package:flutter/foundation.dart' show kIsWeb;
import 'package:flutter/material.dart';
import 'package:google_sign_in/google_sign_in.dart';
import '../services/api_service.dart';

class LoginScreen extends StatefulWidget {
  const LoginScreen({Key? key}) : super(key: key);
  @override
  State<LoginScreen> createState() => _LoginScreenState();
}

class _LoginScreenState extends State<LoginScreen> {
  final _formKey = GlobalKey<FormState>();
  final TextEditingController _emailController = TextEditingController();
  final TextEditingController _passwordController = TextEditingController();
  final ApiService _apiService = ApiService();
  bool _isLoading = false;
  // radio button state for user type
  String _selectedType = 'patient';

  // Replace this with your Web client ID (OAuth2 → Web application) from Cloud Console
  static const String WEB_CLIENT_ID =
      '67750453943-d784cdhm13nbo13qimepoegbqeakfgdg.apps.googleusercontent.com';

  // 1) Initialize GoogleSignIn so that:
  //    • On web: clientId = WEB_CLIENT_ID (plugin picks up the meta tag, but we pass it explicitly)
  //    • On Android: serverClientId = WEB_CLIENT_ID (so account.authentication.idToken is non-null)
  late final GoogleSignIn _googleSignIn = GoogleSignIn(
    clientId: kIsWeb ? WEB_CLIENT_ID : null,
    serverClientId: kIsWeb ? null : WEB_CLIENT_ID,
    scopes: ['email'],
  );

  Future<void> _handleGoogleSignIn() async {
    setState(() => _isLoading = true);
    try {
      // 2) Call signIn() on all platforms (despite the web deprecation warning).
      //    On web you’ll see a “deprecated” notice, but it still works.
      //    On Android, because we set serverClientId, it will return an idToken.
      print("\nBefore signIn()");
      final account = await _googleSignIn.signIn();
      print("\nAfter signIn(): account = $account");

      if (account == null) {
        // User cancelled the sign-in dialog
        return;
      }

      // 3) Fetch the idToken. On Android, serverClientId ensures this is non-null.
      //    On Web, the plugin will grab it from the meta tag / JS flow.
      final googleAuth = await account.authentication;
      final String? idToken = googleAuth.idToken;
      print("Fetched idToken = $idToken");

      if (idToken == null) {
        throw Exception('No ID token from Google.');
      }

      // 4) Send the idToken to your backend:
      final result = await _apiService.loginWithGoogle(idToken, 'patient');
      // result = { 'token': yourAppJwt, 'role': 'patient', 'id': 'mongodbObjectId' }


      Navigator.of(context).pushReplacementNamed(
        '/home',
        arguments: {
          'role': result['role'],
          'id':   result['id'],
        },
      );
    } catch (e) {
      print("Google Sign-In failed: $e");
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Google Sign-In failed: $e')),
      );
    } finally {
      setState(() => _isLoading = false);
    }
  }

  Future<void> _submitLogin() async {
    if (!_formKey.currentState!.validate()) return;

    setState(() => _isLoading = true);
    try {
      await _apiService.login(
        _emailController.text.trim(),
        _passwordController.text.trim(),
        _selectedType,
      );

      // If login succeeded, navigate to home
      if (!mounted) return;
      Navigator.pushReplacementNamed(context, '/home');
    } catch (e) {
      // Show error message
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text(e.toString())),
      );
    } finally {
      setState(() => _isLoading = false);
    }
  }

  @override
  void dispose() {
    _emailController.dispose();
    _passwordController.dispose();
    super.dispose();
  }

@override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Clinic Login')),
      body: Center(
        child: SingleChildScrollView(
          padding: const EdgeInsets.symmetric(horizontal: 24),
          child: Form(
            key: _formKey,
            child: Column(
              children: [
                const SizedBox(height: 48),

                // Radio buttons for selecting user type
                Row(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    Radio<String>(
                      value: 'patient',
                      groupValue: _selectedType,
                      onChanged: (value) {
                        setState(() {
                          _selectedType = value!;
                        });
                      },
                    ),
                    const Text('Patient'),
                    const SizedBox(width: 16),
                    Radio<String>(
                      value: 'doctor',
                      groupValue: _selectedType,
                      onChanged: (value) {
                        setState(() {
                          _selectedType = value!;
                        });
                      },
                    ),
                    const Text('Doctor'),
                  ],
                ),

                const SizedBox(height: 24),
                TextFormField(
                  controller: _emailController,
                  decoration: const InputDecoration(
                    labelText: 'Email',
                    border: OutlineInputBorder(),
                  ),
                  keyboardType: TextInputType.emailAddress,
                  validator: (value) {
                    if (value == null || value.isEmpty) {
                      return 'Please enter your email';
                    }
                    if (!value.contains('@')) {
                      return 'Enter a valid email';
                    }
                    return null;
                  },
                ),
                const SizedBox(height: 16),
                TextFormField(
                  controller: _passwordController,
                  decoration: const InputDecoration(
                    labelText: 'Password',
                    border: OutlineInputBorder(),
                  ),
                  obscureText: true,
                  validator: (value) {
                    if (value == null || value.isEmpty) {
                      return 'Please enter your password';
                    }
                    if (value.length < 6) {
                      return 'Password must be at least 6 characters';
                    }
                    return null;
                  },
                ),
                const SizedBox(height: 24),
                ElevatedButton(
                  onPressed: _isLoading ? null : _submitLogin,
                  style: ElevatedButton.styleFrom(
                    minimumSize: const Size.fromHeight(48),
                  ),
                  child: _isLoading
                      ? const SizedBox(
                          height: 16,
                          width: 16,
                          child: CircularProgressIndicator(strokeWidth: 2),
                        )
                      : const Text('Log In'),
                ),
                const SizedBox(height: 12),
                TextButton(
                  onPressed: () {
                    Navigator.pushNamed(context, '/register');
                  },
                  child: const Text('Create an account'),
                ),
                const SizedBox(height: 12),
                 // Google Sign-In button
                 _isLoading
                  ? const CircularProgressIndicator()
                  : InkWell(
                      onTap: _handleGoogleSignIn,
                      child: Image.asset(
                        'assets/google_logo.png',
                        fit: BoxFit.contain,
                      ),
                    ),
              ],
            ),
          ),
        ),
      ),
    );
  }
}

