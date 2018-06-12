# frozen_string_literal: true

class Users::SessionsController < Devise::SessionsController
  # before_action :configure_sign_in_params, only: [:create]

  # GET /resource/sign_in
  # def new
  #   super
  # end

  # POST /resource/sign_in
  # def create
  #   super
  # end

  # DELETE /resource/sign_out
  # def destroy
  #   super
  # end

  # protected

  # If you have extra params to permit, append them to the sanitizer.
  # def configure_sign_in_params
  #   devise_parameter_sanitizer.permit(:sign_in, keys: [:attribute])
  # end
def edit
    @user = current_user


  end

  def update
   @user = current_user
   @user.update(user_params)


   if @user.update!(user_params)
    redirect_to new_trainer_path(@user)
  else
    render :edit
  end

end

private

def user_params
  params.require(:user).permit(:first_name, :last_name, :age, :gender)
end


end
