import { useState } from 'react';
import { useForm } from 'react-hook-form';
import { yupResolver } from '@hookform/resolvers/yup';
import * as yup from 'yup';
import { Monitor, Eye, EyeOff, User, Lock, AlertCircle, CheckCircle, LogIn } from 'lucide-react';
import Swal from 'sweetalert2';
import { useNavigate } from 'react-router-dom';
import axios from 'axios';

const esquemaValidacion = yup.object({
    nombreUsuario: yup
        .string()
        .required('El nombre de usuario es requerido.')
        .min(3, 'Mínimo 3 caracteres.')
        .max(20, 'Máximo 20 caracteres.'),
    contrasena: yup
        .string()
        .required('La contraseña es requerida.')
        .min(3, 'Debe tener al menos 3 caracteres.')
});

export default function Login() {
    const [mostrarContrasena, setMostrarContrasena] = useState(false);
    const [cargando, setCargando] = useState(false);
    const navigate = useNavigate();

    const {
        register,
        handleSubmit,
        formState: { errors, isValid, touchedFields },
        watch
    } = useForm({
        resolver: yupResolver(esquemaValidacion),
        mode: 'onChange'
    });

    const valoresFormulario = watch();

    const alEnviar = async (datos) => {
        setCargando(true);

        try {
            const { data } = await axios.post('http://localhost:8081/login', {
                usuario: datos.nombreUsuario,
                contrasena: datos.contrasena
            });

            if (data && data.usuario && data.grupo) {
                Swal.fire({
                    icon: 'success',
                    title: 'Bienvenido a USACLinux',
                    text: 'Inicio de sesión exitoso.',
                    confirmButtonColor: '#059669'
                });
                localStorage.setItem('usuario', JSON.stringify(data.usuario));
                localStorage.setItem('grupo', JSON.stringify(data.grupo));
                navigate('/dashboard');
            } else {
                Swal.fire({
                    icon: 'error',
                    title: 'Error al iniciar sesión',
                    text: data.mensaje || 'Credenciales inválidas',
                    confirmButtonColor: '#dc2626'
                });
            }
        } catch (error) {
            console.error('Error al iniciar sesión:', error);
            Swal.fire({
                icon: 'error',
                title: 'Error de conexión',
                text: 'No se pudo conectar al servidor',
                confirmButtonColor: '#dc2626'
            });
        } finally {
            setCargando(false);
        }
    };


    const obtenerEstadoCampo = (nombreCampo) => {
        const tieneError = errors[nombreCampo];
        const tocado = touchedFields[nombreCampo];
        const valor = valoresFormulario[nombreCampo];
        if (tieneError && tocado) return 'error';
        if (!tieneError && tocado && valor) return 'valido';
        return 'normal';
    };

    return (
        <div className="min-h-screen bg-gradient-to-br from-zinc-50 to-zinc-100 flex items-center justify-center p-4">
            <div className="bg-white rounded-2xl shadow-xl w-full max-w-md p-8 transform transition-all duration-300 hover:shadow-2xl">
                
                {/* Header con logo y título */}
                <div className="text-center mb-8">
                    <div className="bg-emerald-900 w-20 h-20 rounded-full flex items-center justify-center mx-auto mb-4 shadow-lg shadow-emerald-900/30">
                        <Monitor className="w-10 h-10 text-white" />
                    </div>
                    <h1 className="text-3xl font-bold text-zinc-900">USACLinux</h1>
                    <p className="text-zinc-600 mt-2">Escritorio Remoto Web</p>
                </div>

                <form onSubmit={handleSubmit(alEnviar)} className="space-y-6">
                    {/* Campo Nombre de Usuario */}
                    <div className="relative">
                        <label className="block text-sm font-semibold text-zinc-700 mb-2">
                            Nombre De Usuario
                        </label>
                        <div className="relative">
                            <User className={`absolute left-3 top-1/2 transform -translate-y-1/2 w-5 h-5 transition-colors z-10 ${
                                obtenerEstadoCampo('nombreUsuario') === 'error' ? 'text-red-400' :
                                obtenerEstadoCampo('nombreUsuario') === 'valido' ? 'text-emerald-600' : 'text-zinc-400'
                            }`} />
                            <input
                                type="text"
                                placeholder="Ingresa tu usuario"
                                {...register('nombreUsuario')}
                                className={`w-full pl-12 pr-12 py-4 rounded-xl border-2 transition-all duration-300 ${
                                    obtenerEstadoCampo('nombreUsuario') === 'error'
                                        ? 'border-red-300 focus:border-red-500 bg-red-50'
                                        : obtenerEstadoCampo('nombreUsuario') === 'valido'
                                        ? 'border-emerald-300 focus:border-emerald-500 bg-emerald-50'
                                        : 'border-zinc-200 focus:border-emerald-500 hover:border-emerald-500'
                                } focus:outline-none transform hover:scale-[1.02] focus:scale-[1.02]`}
                            />
                            {obtenerEstadoCampo('nombreUsuario') === 'valido' && (
                                <CheckCircle className="absolute right-3 top-1/2 transform -translate-y-1/2 w-5 h-5 text-emerald-600 z-20" />
                            )}
                            {obtenerEstadoCampo('nombreUsuario') === 'error' && (
                                <AlertCircle className="absolute right-3 top-1/2 transform -translate-y-1/2 w-5 h-5 text-red-500 z-20" />
                            )}
                        </div>
                        {errors.nombreUsuario && (
                            <div className="flex items-center mt-2 text-red-600">
                                <AlertCircle className="w-4 h-4 mr-2" />
                                <p className="text-sm">{errors.nombreUsuario.message}</p>
                            </div>
                        )}
                    </div>

                    {/* Campo Contraseña */}
                    <div className="relative">
                        <label className="block text-sm font-semibold text-zinc-700 mb-2">
                            Contraseña
                        </label>
                        <div className="relative">
                            <Lock className={`absolute left-3 top-1/2 transform -translate-y-1/2 w-5 h-5 transition-colors z-10 ${
                                obtenerEstadoCampo('contrasena') === 'error' ? 'text-red-400' :
                                obtenerEstadoCampo('contrasena') === 'valido' ? 'text-emerald-600' : 'text-zinc-400'
                            }`} />
                            <input
                                type={mostrarContrasena ? 'text' : 'password'}
                                placeholder="Ingresa tu contraseña"
                                {...register('contrasena')}
                                className={`w-full pl-12 pr-16 py-4 rounded-xl border-2 transition-all duration-300 ${
                                    obtenerEstadoCampo('contrasena') === 'error'
                                        ? 'border-red-300 focus:border-red-500 bg-red-50'
                                        : obtenerEstadoCampo('contrasena') === 'valido'
                                        ? 'border-emerald-300 focus:border-emerald-500 bg-emerald-50'
                                        : 'border-zinc-200 focus:border-emerald-500 hover:border-emerald-500'
                                } focus:outline-none transform hover:scale-[1.02] focus:scale-[1.02]`}
                            />
                            <button
                                type="button"
                                onClick={() => setMostrarContrasena(!mostrarContrasena)}
                                className="absolute right-3 top-1/2 transform -translate-y-1/2 text-zinc-400 hover:text-zinc-600 transition-colors z-30"
                            >
                                {mostrarContrasena ? <EyeOff className="w-5 h-5" /> : <Eye className="w-5 h-5" />}
                            </button>
                        </div>
                        {errors.contrasena && (
                            <div className="flex items-center mt-2 text-red-600">
                                <AlertCircle className="w-4 h-4 mr-2" />
                                <p className="text-sm">{errors.contrasena.message}</p>
                            </div>
                        )}
                    </div>

                    {/* Botón de inicio de sesión */}
                    <button
                        type="submit"
                        disabled={cargando || !isValid}
                        className={`w-full py-4 rounded-xl font-bold text-lg transition-all duration-300 transform flex items-center justify-center ${
                            cargando || !isValid
                                ? 'bg-zinc-300 text-zinc-500 cursor-not-allowed'
                                : 'bg-emerald-900 text-white hover:bg-emerald-800 hover:scale-[1.02] shadow-lg shadow-emerald-900/30 hover:shadow-xl hover:shadow-emerald-900/40'
                        }`}
                    >
                        {cargando ? (
                            <div className="flex items-center justify-center">
                                <div className="animate-spin rounded-full h-5 w-5 border-b-2 border-white mr-3"></div>
                                Iniciando Sesión...
                            </div>
                        ) : (
                            <>
                                <LogIn className="w-5 h-5 mr-2" />
                                Iniciar Sesión
                            </>
                        )}
                    </button>
                </form>
            </div>
        </div>
    );
}