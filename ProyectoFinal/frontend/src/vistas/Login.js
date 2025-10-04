import { useState } from 'react';
import { useForm } from 'react-hook-form';
import { yupResolver } from '@hookform/resolvers/yup';
import * as yup from 'yup';
import { ChefHat, Eye, EyeOff, User, Lock, AlertCircle, CheckCircle, LogIn } from 'lucide-react';
import Swal from 'sweetalert2';
import { useNavigate } from 'react-router-dom';
import axios from 'axios';
const API_URL = `http://`;

const esquemaValidacion = yup.object({
    nombreUsuario: yup
        .string()
        .required('El nombre de usuario es requerido.')
        .min(3, 'Mínimo 3 caracteres.')
        .max(20, 'Máximo 20 caracteres.'),
    contrasena: yup
        .string()
        .required('La contraseña es requerida.')
        .min(6, 'Debe tener al menos 6 caracteres.')
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
            const respuesta = await axios.post(API_URL + "/iniciar-sesion", {
                nombre_usuario: datos.nombreUsuario,
                contrasena: datos.contrasena
            });

            if (respuesta.data.success) {
                Swal.fire({
                    icon: 'success',
                    title: 'Bienvenido A RecipeBoxCloud',
                    text: 'Inicio De Sesión Exitoso.',
                    confirmButtonColor: '#1d4ed8'
                });
                console.log(respuesta.data.datos_usuario);
                localStorage.setItem('usuario', JSON.stringify(respuesta.data.datos_usuario));
                navigate('/cliente');
            }
        } catch (error) {
            console.error('Error al iniciar sesión:', error);
            if (error.response) {
                Swal.fire({
                    icon: 'error',
                    title: 'Error Al Iniciar Sesión',
                    text: error.response.data.mensaje,
                    confirmButtonColor: '#d81d1dff'
                });
            } else {
                Swal.fire({
                    icon: 'error',
                    title: 'Error de Conexión',
                    text: 'No se pudo conectar al servidor',
                    confirmButtonColor: '#d81d1dff'
                });
            }
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

    const manejarRecuperarContrasena = () => {
        Swal.fire({
            icon: 'info',
            title: 'Recuperar Contraseña',
            text: 'Contáctate con el soporte para recuperar tu contraseña.',
            confirmButtonColor: '#1d4ed8'
        });
    };

    return (
        <div className="min-h-screen bg-gradient-to-br from-slate-50 to-slate-100 flex items-center justify-center p-4">
            <div className="bg-white rounded-2xl shadow-xl w-full max-w-md p-8 transform transition-all duration-300 hover:shadow-2xl">
                
                {/* Header con logo y título */}
                <div className="text-center mb-8">
                    <div className="bg-slate-900 w-20 h-20 rounded-full flex items-center justify-center mx-auto mb-4 shadow-lg">
                        <ChefHat className="w-10 h-10 text-white" />
                    </div>
                    <h1 className="text-3xl font-bold text-slate-900">RecipeBoxCloud</h1>
                    <p className="text-slate-600 mt-2">Inicia Sesión En Tu Cuenta</p>
                </div>

                <form onSubmit={handleSubmit(alEnviar)} className="space-y-6">
                    {/* Campo Nombre de Usuario */}
                    <div className="relative">
                        <label className="block text-sm font-semibold text-slate-700 mb-2">
                            Nombre De Usuario
                        </label>
                        <div className="relative">
                            <User className={`absolute left-3 top-1/2 transform -translate-y-1/2 w-5 h-5 transition-colors z-10 ${
                                obtenerEstadoCampo('nombreUsuario') === 'error' ? 'text-red-400' :
                                obtenerEstadoCampo('nombreUsuario') === 'valido' ? 'text-blue-600' : 'text-slate-400'
                            }`} />
                            <input
                                type="text"
                                placeholder="Ingresa tu usuario"
                                {...register('nombreUsuario')}
                                className={`w-full pl-12 pr-12 py-4 rounded-xl border-2 transition-all duration-300 ${
                                    obtenerEstadoCampo('nombreUsuario') === 'error'
                                        ? 'border-red-300 focus:border-red-500 bg-red-50'
                                        : obtenerEstadoCampo('nombreUsuario') === 'valido'
                                        ? 'border-blue-300 focus:border-blue-500 bg-blue-50'
                                        : 'border-slate-200 focus:border-blue-500 hover:border-slate-500'
                                } focus:outline-none transform hover:scale-[1.02] focus:scale-[1.02]`}
                            />
                            {obtenerEstadoCampo('nombreUsuario') === 'valido' && (
                                <CheckCircle className="absolute right-3 top-1/2 transform -translate-y-1/2 w-5 h-5 text-blue-600 z-20" />
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
                        <label className="block text-sm font-semibold text-slate-700 mb-2">
                            Contraseña
                        </label>
                        <div className="relative">
                            <Lock className={`absolute left-3 top-1/2 transform -translate-y-1/2 w-5 h-5 transition-colors z-10 ${
                                obtenerEstadoCampo('contrasena') === 'error' ? 'text-red-400' :
                                obtenerEstadoCampo('contrasena') === 'valido' ? 'text-blue-600' : 'text-slate-400'
                            }`} />
                            <input
                                type={mostrarContrasena ? 'text' : 'password'}
                                placeholder="Ingresa tu contraseña"
                                {...register('contrasena')}
                                className={`w-full pl-12 pr-16 py-4 rounded-xl border-2 transition-all duration-300 ${
                                    obtenerEstadoCampo('contrasena') === 'error'
                                        ? 'border-red-300 focus:border-red-500 bg-red-50'
                                        : obtenerEstadoCampo('contrasena') === 'valido'
                                        ? 'border-blue-300 focus:border-blue-500 bg-blue-50'
                                        : 'border-slate-200 focus:border-blue-500 hover:border-slate-500'
                                } focus:outline-none transform hover:scale-[1.02] focus:scale-[1.02]`}
                            />
                            <button
                                type="button"
                                onClick={() => setMostrarContrasena(!mostrarContrasena)}
                                className="absolute right-3 top-1/2 transform -translate-y-1/2 text-slate-400 hover:text-slate-600 transition-colors z-30"
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

                    {/* Opciones adicionales */}
                    <div className="flex items-center justify-between">
                        <button
                            type="button"
                            onClick={manejarRecuperarContrasena}
                            className="text-sm text-blue-700 hover:text-blue-800 hover:underline transition-colors"
                        >
                            ¿Olvidaste Tu Contraseña?
                        </button>
                    </div>

                    {/* Botón de inicio de sesión */}
                    <button
                        type="submit"
                        disabled={cargando || !isValid}
                        className={`w-full py-4 rounded-xl font-bold text-lg transition-all duration-300 transform flex items-center justify-center ${
                            cargando || !isValid
                                ? 'bg-slate-300 text-slate-500 cursor-not-allowed'
                                : 'bg-slate-900 text-white hover:bg-slate-800 hover:scale-[1.02] shadow-lg hover:shadow-xl'
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

                    {/* Separador */}
                    <div className="flex items-center my-6">
                        <div className="flex-1 border-t border-slate-200"></div>
                        <span className="px-4 text-sm text-slate-500 bg-white">o</span>
                        <div className="flex-1 border-t border-slate-200"></div>
                    </div>

                    {/* Link a registro */}
                    <div className="text-center">
                        <p className="text-slate-600">
                            ¿No Tienes Cuenta?{' '}
                            <button
                                type="button"
                                className="text-blue-700 hover:text-blue-800 font-bold transition-colors hover:underline"
                                onClick={() => navigate('/registro')}
                            >
                                Regístrate Aquí
                            </button>
                        </p>
                    </div>
                </form>
            </div>
        </div>
    );
}