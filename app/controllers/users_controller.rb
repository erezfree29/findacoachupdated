class UsersController < ApplicationController
  def index
  end

  def show
  end

  def new
  end

  def create
  end

  def edit
    @user = current_user


  end

  def update
   @user = current_user


   if @user.update!(user_params)
    redirect_to new_trainer_path(@user)
  else
    render :edit
  end

end

def destroy
end

private

def user_params
  params.require(:user).permit(:first_name, :last_name, :age, :gender)
end



end
